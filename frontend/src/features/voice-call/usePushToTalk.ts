import { useEffect, useState } from "react";
import { toast } from "react-toastify";
import { startPushToTalk, stopPushToTalk } from "../../api/pushToTalk";
import { getErrorMessage } from "./getErrorMessage";

type UsePushToTalkOptions = {
  bridgeId: string | null;
  sessionId: string | null;
  enabled: boolean;
};

export function usePushToTalk({
  bridgeId,
  sessionId,
  enabled,
}: UsePushToTalkOptions) {
  const [isPressed, setIsPressed] = useState(false);

  useEffect(() => {
    if (!enabled || !bridgeId || !sessionId) {
      return;
    }

    const heldKeys = new Set<string>();
    let pressed = false;
    let started = false;
    let disposed = false;

    const stop = () => {
      if (!pressed) return;

      pressed = false;
      if (!disposed) setIsPressed(false);
      if (!started) return;

      started = false;
      void stopPushToTalk(bridgeId, sessionId).catch((error: unknown) => {
        if (!disposed) {
          toast.error(getErrorMessage(error, "Could not stop push to talk"));
        }
      });
    };

    const start = () => {
      if (pressed) return;

      pressed = true;
      setIsPressed(true);
      void startPushToTalk(bridgeId, sessionId)
        .then(() => {
          started = true;
          // A quick release can happen before /ptt/start returns. Complete the
          // matching stop once start is acknowledged in that case.
          if (!pressed || disposed) {
            started = false;
            void stopPushToTalk(bridgeId, sessionId).catch((error: unknown) => {
              if (!disposed) {
                toast.error(
                  getErrorMessage(error, "Could not stop push to talk"),
                );
              }
            });
          }
        })
        .catch((error: unknown) => {
          pressed = false;
          if (!disposed) {
            setIsPressed(false);
            toast.error(getErrorMessage(error, "Could not start push to talk"));
          }
        });
    };

    const onKeyDown = (event: KeyboardEvent) => {
      if (event.code === "ControlLeft" || event.code === "ControlRight") {
        heldKeys.add("Control");
      }
      if (event.code === "Space") heldKeys.add("Space");

      if (heldKeys.has("Control") && heldKeys.has("Space")) {
        event.preventDefault();
        start();
      }
    };

    const onKeyUp = (event: KeyboardEvent) => {
      if (event.code === "ControlLeft" || event.code === "ControlRight") {
        heldKeys.delete("Control");
      }
      if (event.code === "Space") heldKeys.delete("Space");
      if (pressed && (!heldKeys.has("Control") || !heldKeys.has("Space"))) {
        event.preventDefault();
        stop();
      }
    };

    const release = () => {
      heldKeys.clear();
      stop();
    };
    const onVisibilityChange = () => {
      if (document.hidden) release();
    };

    window.addEventListener("keydown", onKeyDown);
    window.addEventListener("keyup", onKeyUp);
    window.addEventListener("blur", release);
    document.addEventListener("visibilitychange", onVisibilityChange);

    return () => {
      release();
      disposed = true;
      window.removeEventListener("keydown", onKeyDown);
      window.removeEventListener("keyup", onKeyUp);
      window.removeEventListener("blur", release);
      document.removeEventListener("visibilitychange", onVisibilityChange);
    };
  }, [bridgeId, enabled, sessionId]);

  return { isPressed: enabled && Boolean(bridgeId && sessionId) && isPressed };
}
