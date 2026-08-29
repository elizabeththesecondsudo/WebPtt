import { z } from 'zod'

const backendUrlSchema = z
  .url('VITE_BACKEND_URL must be a valid URL')
  .default('http://localhost:8080')
  .transform((url) => url.replace(/\/$/, ''))

export const config = Object.freeze({
  backendUrl: backendUrlSchema.parse(import.meta.env.VITE_BACKEND_URL),
})
