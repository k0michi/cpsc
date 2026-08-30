import { Link } from "react-router";
import type { Route } from "./+types/snippets.$slug";
import { AppShell } from "../components/app-shell";
import { CodeBlock } from "../components/code-block";
import { MarkdownContent, TextBlock } from "../components/text-block";
import { findSnippet } from "../data/snippets";

export function loader({ params }: Route.LoaderArgs) {
  const snippet = findSnippet(params.slug);
  if (!snippet) throw new Response("Not Found", { status: 404 });
  return { snippet };
}
export function meta({ loaderData }: Route.MetaArgs) {
  return [{ title: `${loaderData?.snippet.title ?? "Snippet"} — CPSC` }];
}
export default function SnippetPage({ loaderData }: Route.ComponentProps) {
  const { snippet } = loaderData;
  const testsPassed = import.meta.env.VITE_TEST_STATUS === "passed";
  return <AppShell category={snippet.category}>
    <article className="detail">
      <Link to="/" className="back-link">← ALL SNIPPETS</Link>
      <header className="detail-header">
        <div><p className="eyebrow lime">{snippet.category.toUpperCase()}</p><h1>{snippet.title}</h1></div>
        <span className="language-badge">{snippet.language.name} {snippet.language.version}</span>
      </header>
      <MarkdownContent source={snippet.description} className="lead" />
      <div className="notebook">
        {snippet.blocks.map((block, index) => {
          if (block.type === "text") {
            return <TextBlock source={block.source} key={`text:${index}`} />;
          }
          const subsnippet = snippet.subsnippets[block.subsnippetIndex];
          return (
          <section className="subsnippet" key={`code:${block.subsnippetIndex}`}>
            <header className="code-section-heading">
              <h2>
                <span>{String(block.subsnippetIndex + 1).padStart(2, "0")}</span>
                {subsnippet.title}
              </h2>
            </header>
            <MarkdownContent
              source={subsnippet.description}
              className="subsnippet-description"
            />
            <CodeBlock
              code={subsnippet.code}
              filename={snippet.sourceFile}
              language={snippet.language}
            />
          </section>
          );
        })}
      </div>
      {snippet.validation && snippet.validation.length > 0 && (
        <section className="validation-section">
          <header className="test-heading">
            <h2>Validation</h2>
            <span>{snippet.validation.length}</span>
          </header>
          <div className="validation-list">
            {snippet.validation.map((validation) => (
              <a
                href={validation.url}
                target="_blank"
                rel="noreferrer"
                key={validation.url}
              >
                <span>✓</span>
                {validation.label}
                <b>↗</b>
              </a>
            ))}
          </div>
        </section>
      )}
      {snippet.tests.length > 0 && <section className="test-section">
        <header className="test-heading">
          <h2>Test Cases</h2>
          <span>{snippet.tests.length}</span>
        </header>
        <div className="test-list">
          {snippet.tests.map((test, index) => (
            <section className="test-case" key={`${test.sourceFile}:${test.title}`}>
              <header className="code-section-heading">
                <h3><span>{String(index + 1).padStart(2, "0")}</span>{test.title}</h3>
                <span className={`test-status ${testsPassed ? "passed" : "unknown"}`}>
                  {testsPassed ? "✓ PASSED" : "? UNKNOWN"}
                </span>
              </header>
              <CodeBlock
                code={test.code}
                filename={test.sourceFile}
                language={snippet.language}
              />
            </section>
          ))}
        </div>
      </section>}
    </article>
  </AppShell>;
}
