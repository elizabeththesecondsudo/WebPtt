import { useCallback, useEffect, useRef, useState } from 'react'
import { config } from '../config'
import {
  websocketMessageSchema,
  type WebSocketMessage,
} from '../api/websocketMessages'

export type WebSocketStatus =
  | 'connecting'
  | 'connected'
  | 'disconnected'
  | 'error'

const reconnectDelayMs = 5_000

export function useBackendWebSocket() {
  const socketRef = useRef<WebSocket | null>(null)
  const messageListenersRef = useRef(
    new Set<(message: WebSocketMessage) => void>(),
  )
  const pendingMessagesRef = useRef<WebSocketMessage[]>([])
  const [status, setStatus] = useState<WebSocketStatus>('connecting')
  const [sessionId, setSessionId] = useState<string | null>(null)
  const [lastMessage, setLastMessage] = useState<WebSocketMessage | null>(null)

  useEffect(() => {
    let active = true
    let reconnectTimer: ReturnType<typeof setTimeout> | undefined

    const connect = () => {
      if (!active) return

      setStatus('connecting')
      pendingMessagesRef.current = []
      const socket = new WebSocket(config.websocketUrl)
      socketRef.current = socket

      socket.addEventListener('open', () => {
        if (active) setStatus('connected')
      })

      socket.addEventListener('message', (event) => {
        if (!active || typeof event.data !== 'string') return

        try {
          const message = websocketMessageSchema.safeParse(JSON.parse(event.data))
          if (!message.success) return

          setLastMessage(message.data)
          if (messageListenersRef.current.size === 0) {
            pendingMessagesRef.current.push(message.data)
          } else {
            messageListenersRef.current.forEach((listener) => listener(message.data))
          }
          if (message.data.type_ === 'session_created') {
            setSessionId(message.data.session_id_)
          }
        } catch {
          // Ignore malformed messages rather than closing a healthy connection.
        }
      })

      socket.addEventListener('error', () => {
        if (active) setStatus('error')
      })

      socket.addEventListener('close', () => {
        if (!active) return

        socketRef.current = null
        setStatus('disconnected')
        setSessionId(null)
        reconnectTimer = setTimeout(connect, reconnectDelayMs)
      })
    }

    connect()

    return () => {
      active = false
      clearTimeout(reconnectTimer)
      const activeSocket = socketRef.current
      socketRef.current = null
      activeSocket?.close()
    }
  }, [])

  const send = useCallback((message: WebSocketMessage) => {
    if (socketRef.current?.readyState !== WebSocket.OPEN) return false

    socketRef.current.send(JSON.stringify(message))
    return true
  }, [])

  const subscribe = useCallback(
    (listener: (message: WebSocketMessage) => void) => {
      messageListenersRef.current.add(listener)
      pendingMessagesRef.current.splice(0).forEach(listener)
      return () => messageListenersRef.current.delete(listener)
    },
    [],
  )

  return { status, sessionId, lastMessage, send, subscribe }
}
