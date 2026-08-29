import { useIsAlive } from './hooks/useIsAlive'
import { useBackendWebSocket } from './hooks/useBackendWebSocket'

function App() {
  const { data: isAlive, isPending } = useIsAlive()
  const websocket = useBackendWebSocket()

  return (
    <main className="flex min-h-screen items-center justify-center bg-slate-950 p-6 text-slate-100">
      <section className="w-full max-w-lg space-y-5 rounded-2xl border border-slate-800 bg-slate-900 p-6 shadow-xl">
        <div>
          <h1 className="text-xl font-semibold">WebPTT connection</h1>
          <p className="mt-1 text-sm text-slate-400">C++ backend status</p>
        </div>

        <dl className="divide-y divide-slate-800 rounded-xl border border-slate-800">
          <div className="flex items-center justify-between p-4">
            <dt className="text-sm text-slate-400">HTTP API</dt>
            <dd className="text-sm font-medium">
              {isPending ? 'Checking…' : isAlive ? 'Connected' : 'Disconnected'}
            </dd>
          </div>
          <div className="flex items-center justify-between p-4">
            <dt className="text-sm text-slate-400">WebSocket</dt>
            <dd className="text-sm font-medium capitalize">{websocket.status}</dd>
          </div>
          <div className="flex items-center justify-between gap-4 p-4">
            <dt className="shrink-0 text-sm text-slate-400">Session ID</dt>
            <dd className="truncate font-mono text-xs text-slate-300">
              {websocket.sessionId ?? 'Waiting for backend…'}
            </dd>
          </div>
        </dl>
      </section>
    </main>
  )
}

export default App
