import { Radio } from "lucide-react";

export type AppHeaderProps = {
  isConnected: boolean;
};

export function AppHeader({ isConnected }: AppHeaderProps) {
  return (
    <div className="flex items-center gap-3 border-b border-black/20 bg-[#313338] px-6 py-5">
      <div className="grid size-11 place-items-center rounded-full bg-indigo-500 shadow-lg shadow-indigo-950/30">
        <Radio aria-hidden="true" className="size-5" />
      </div>
      <div className="min-w-0 flex-1">
        <h1 className="font-semibold tracking-tight">WebPTT Voice</h1>
        <p className="text-sm text-[#b5bac1]">
          Private browser-to-browser audio
        </p>
      </div>
      <span
        className={`size-2.5 rounded-full ${
          isConnected
            ? "bg-emerald-400 shadow-[0_0_10px_#34d399]"
            : "bg-rose-400"
        }`}
        title="Connection status"
      />
    </div>
  );
}
