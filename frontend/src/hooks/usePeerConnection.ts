import { useEffect, useRef, useState } from 'react'
import type { WebSocketMessage } from '../api/websocketMessages'
import type { WebSocketStatus } from './useBackendWebSocket'

export type PeerConnectionStatus =
  | 'idle'
  | 'connecting'
  | 'connected'
  | 'disconnected'
  | 'failed'
  | 'closed'

type UsePeerConnectionOptions = {
  microphoneStream: MediaStream | null
  websocketStatus: WebSocketStatus
  sendSignal: (message: WebSocketMessage) => boolean
  subscribeToSignals: (
    listener: (message: WebSocketMessage) => void,
  ) => () => void
}

export function usePeerConnection({
  microphoneStream,
  websocketStatus,
  sendSignal,
  subscribeToSignals,
}: UsePeerConnectionOptions) {
  const peerRef = useRef<RTCPeerConnection | null>(null)
  const audioSenderRef = useRef<RTCRtpSender | null>(null)
  const placeholderTrackRef = useRef<MediaStreamTrack | null>(null)
  const pendingCandidatesRef = useRef<RTCIceCandidateInit[]>([])
  const signalingChainRef = useRef(Promise.resolve())
  const [status, setStatus] = useState<PeerConnectionStatus>('idle')
  const [remoteStream, setRemoteStream] = useState<MediaStream | null>(null)

  useEffect(() => {
    if (websocketStatus !== 'connected') return

    const peer = new RTCPeerConnection({ iceServers: [] })
    const remoteAudio = new MediaStream()
    // Negotiate a sender SSRC in the first offer. libdatachannel routes RTP by
    // the SSRC advertised in SDP, while replaceTrack intentionally does not
    // renegotiate. A silent placeholder lets the real microphone be swapped in
    // later without changing the negotiated RTP source.
    const audioContext = new AudioContext()
    const silentSource = audioContext.createConstantSource()
    const silentGain = audioContext.createGain()
    const silentDestination = audioContext.createMediaStreamDestination()
    silentGain.gain.value = 0
    silentSource.connect(silentGain).connect(silentDestination)
    silentSource.start()
    const placeholderTrack = silentDestination.stream.getAudioTracks()[0]
    const audioTransceiver = peer.addTransceiver(placeholderTrack, {
      direction: 'sendrecv',
    })

    peerRef.current = peer
    audioSenderRef.current = audioTransceiver.sender
    placeholderTrackRef.current = placeholderTrack
    pendingCandidatesRef.current = []
    signalingChainRef.current = Promise.resolve()

    peer.addEventListener('connectionstatechange', () => {
      setStatus(peer.connectionState as PeerConnectionStatus)
    })

    peer.addEventListener('track', (event) => {
      event.streams[0]?.getAudioTracks().forEach((track) => {
        if (!remoteAudio.getTrackById(track.id)) remoteAudio.addTrack(track)
      })

      if (event.streams.length === 0 && event.track.kind === 'audio') {
        remoteAudio.addTrack(event.track)
      }
      setRemoteStream(new MediaStream(remoteAudio.getTracks()))
    })

    peer.addEventListener('icecandidate', (event) => {
      if (!event.candidate) return

      sendSignal({
        type_: 'candidate',
        candidate_: event.candidate.candidate,
        sdp_mid_: event.candidate.sdpMid ?? '0',
      })
    })

    const negotiate = () => {
      signalingChainRef.current = signalingChainRef.current
        .then(async () => {
          if (peer.signalingState !== 'stable') return
          const offer = await peer.createOffer()
          await peer.setLocalDescription(offer)
          if (peer.localDescription?.type === 'offer') {
            sendSignal({
              type_: 'offer',
              sdp_: peer.localDescription.sdp,
            })
          }
        })
        .catch(() => setStatus('failed'))
    }
    peer.addEventListener('negotiationneeded', negotiate)

    const applyPendingCandidates = async () => {
      const candidates = pendingCandidatesRef.current.splice(0)
      for (const candidate of candidates) await peer.addIceCandidate(candidate)
    }

    const unsubscribe = subscribeToSignals((message) => {
      signalingChainRef.current = signalingChainRef.current
        .then(async () => {
          if (message.type_ === 'session_created') return

          if (message.type_ === 'candidate' || message.type_ === 'ice_candidate') {
            const candidate = {
              candidate: message.candidate_,
              sdpMid: message.sdp_mid_,
            }
            if (!peer.remoteDescription) {
              pendingCandidatesRef.current.push(candidate)
              return
            }
            await peer.addIceCandidate(candidate)
            return
          }

          if (!('sdp_' in message)) return

          const offerCollision =
            message.type_ === 'offer' && peer.signalingState !== 'stable'
          if (offerCollision) await peer.setLocalDescription({ type: 'rollback' })

          await peer.setRemoteDescription({
            type: message.type_,
            sdp: message.sdp_,
          })
          await applyPendingCandidates()

          if (message.type_ === 'offer') {
            const answer = await peer.createAnswer()
            await peer.setLocalDescription(answer)
            if (peer.localDescription?.type === 'answer') {
              sendSignal({
                type_: 'answer',
                sdp_: peer.localDescription.sdp,
              })
            }
          }
        })
        .catch(() => setStatus('failed'))
    })

    return () => {
      unsubscribe()
      peerRef.current = null
      audioSenderRef.current = null
      placeholderTrackRef.current = null
      remoteAudio.getTracks().forEach((track) => track.stop())
      placeholderTrack.stop()
      silentSource.stop()
      void audioContext.close()
      peer.close()
      setRemoteStream(null)
      setStatus('disconnected')
    }
  }, [sendSignal, subscribeToSignals, websocketStatus])

  useEffect(() => {
    const sender = audioSenderRef.current
    if (!sender) return

    const microphoneTrack =
      microphoneStream?.getAudioTracks()[0] ?? placeholderTrackRef.current
    if (!microphoneTrack) return
    void sender.replaceTrack(microphoneTrack).catch(() => setStatus('failed'))
  }, [microphoneStream])

  return { status, remoteStream }
}
