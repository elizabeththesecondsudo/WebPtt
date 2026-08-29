import { useQuery } from '@tanstack/react-query'
import { fetchIsAlive } from '../api/isAlive'

export function useIsAlive() {
  return useQuery({
    queryKey: ['isAlive'],
    queryFn: fetchIsAlive,
    refetchInterval: 5_000,
    refetchIntervalInBackground: true,
    retry: false,
  })
}

