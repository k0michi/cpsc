import Markdown from "react-markdown";
import rehypeKatex from "rehype-katex";
import remarkGfm from "remark-gfm";
import remarkMath from "remark-math";
import "katex/dist/katex.min.css";

type MarkdownContentProps = {
  source?: string | null;
  className?: string;
};

export function MarkdownContent({ source, className }: MarkdownContentProps) {
  if (!source?.trim()) return null;

  return (
    <div className={className}>
      <Markdown
        remarkPlugins={[remarkGfm, remarkMath]}
        rehypePlugins={[rehypeKatex]}
      >
        {source}
      </Markdown>
    </div>
  );
}

export function TextBlock({ source }: { source?: string | null }) {
  if (!source?.trim()) return null;

  return (
    <section className="text-block">
      <MarkdownContent source={source} />
    </section>
  );
}
