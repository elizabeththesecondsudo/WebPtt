import { useIsAlive } from './hooks/useIsAlive'

function App() {
  const { data: isAlive, isPending } = useIsAlive()

  return (
    <main>
      <p>
        Backend status:{' '}
        {isPending ? 'Checking…' : isAlive ? 'Connected' : 'Disconnected'}
      </p>
    </main>
  )
}

export default App
