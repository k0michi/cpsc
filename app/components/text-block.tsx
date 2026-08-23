import Markdown from "react-markdown";
import rehypeKatex from "rehype-katex";
import remarkGfm from "remark-gfm";
import remarkMath from "remark-math";
import "katex/dist/katex.min.css";

export function TextBlock({ source }: { source?: string | null }) {
  if (!source?.trim()) return null;

  return (
    <section className="text-block">
      <Markdown
        remarkPlugins={[remarkGfm, remarkMath]}
        rehypePlugins={[rehypeKatex]}
      >
        {source}
      </Markdown>
    </section>
  );
}
