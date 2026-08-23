import { useMemo, useState } from "react";
import Prism from "prismjs";
import "prismjs/components/prism-clike";
import "prismjs/components/prism-c";
import "prismjs/components/prism-cpp";
import type { Snippet } from "../data/snippets";

type CodeBlockProps = {
  code: string;
  filename: string;
  language: Snippet["language"];
};

export function CodeBlock({ code, filename, language }: CodeBlockProps) {
  const [copied, setCopied] = useState(false);
  const highlightedCode = useMemo(
    () => Prism.highlight(
      code,
      Prism.languages[language.prism] ?? Prism.languages.plain,
      language.prism,
    ),
    [code, language.prism],
  );
  async function copy() {
    await navigator.clipboard.writeText(code);
    setCopied(true);
    window.setTimeout(() => setCopied(false), 1600);
  }
  return (
    <div className="code-window">
      <div className="code-toolbar">
        <div className="window-dots"><i /><i /><i /></div>
        <span>{filename}</span>
        <button type="button" onClick={copy} className={copied ? "copied" : ""} aria-label={`${language.name} codeをコピー`}>
          {copied ? "✓ COPIED" : "▣ COPY"}
        </button>
      </div>
      <pre aria-label={`${language.name} source code`}>
        <code
          className={`language-${language.prism}`}
          dangerouslySetInnerHTML={{ __html: highlightedCode }}
        />
      </pre>
    </div>
  );
}
