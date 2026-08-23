import { Link } from "react-router";
import type { Route } from "./+types/_index";
import { AppShell } from "../components/app-shell";
import { categories, snippets } from "../data/snippets";
import { categoryHueStyle } from "../utils/category-color";

export function meta({}: Route.MetaArgs) {
  return [
    { title: "CPSC — Competitive Programming Snippet Collection" },
    { name: "description", content: "Competitive Programming Snippet Collection" },
  ];
}

export default function Home() {
  return (
    <AppShell>
      <section className="simple-home">
        <header className="simple-heading">
          <h1>Snippets</h1>
          <span>{snippets.length}</span>
        </header>
        <div className="snippet-list">
          {snippets.map((snippet, index) => (
            <Link
              to={`/snippets/${snippet.slug}`}
              className="snippet-row"
              key={snippet.slug}
            >
              <span
                className="row-number"
                style={categoryHueStyle(snippet.category, categories)}
              >
                {String(index + 1).padStart(2, "0")}
              </span>
              <strong>{snippet.title}</strong>
              <span>{snippet.category}</span>
              <b>›</b>
            </Link>
          ))}
        </div>
      </section>
    </AppShell>
  );
}
