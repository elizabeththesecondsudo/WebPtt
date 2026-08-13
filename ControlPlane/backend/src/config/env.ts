import "dotenv/config";
import { z } from "zod";

const envSchema = z.object({
  PORT: z.coerce.number().int().min(1).max(65_535).default(3000),
  MEDIA_PLANE_URL: z.url().default("http://127.0.0.1:8080"),
});

const parsedEnv = envSchema.parse(process.env);

export const env = {
  port: parsedEnv.PORT,
  mediaPlaneUrl: parsedEnv.MEDIA_PLANE_URL.replace(/\/$/, ""),
} as const;
