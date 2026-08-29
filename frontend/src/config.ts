import { z } from 'zod'

const backendAddress = z
  .string()
  .trim()
  .min(1, 'VITE_BACKEND_ADDRESS is required')
  .default('localhost:8080')
  .parse(import.meta.env.VITE_BACKEND_ADDRESS)

const isSecure = window.location.protocol === 'https:'
const httpProtocol = isSecure ? 'https' : 'http'
const websocketProtocol = isSecure ? 'wss' : 'ws'

export const config = Object.freeze({
  backendUrl: z.url().parse(`${httpProtocol}://${backendAddress}`),
  websocketUrl: z
    .url({ protocol: /^wss?$/ })
    .parse(`${websocketProtocol}://${backendAddress}`),
})
