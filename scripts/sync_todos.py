import os, re

root = os.path.dirname(os.path.dirname(__file__))
todo_file = os.path.join(root, "TODO.md")

patterns = [
    r"//\s*(TODO|FIXME|HACK)\s*:(.*)",
    r"/\*\s*(TODO|FIXME|HACK)\s*:(.*?)\*/"
]

todos = []

for dirpath, _, filenames in os.walk(root):
    if any(part in dirpath for part in [".git", "build", ".idea", ".vscode", "__pycache__"]):
        continue
    for fname in filenames:
        if fname.endswith((".cpp", ".h", ".hpp", ".c", ".py", ".ps1", ".cmake")):
            path = os.path.join(dirpath, fname)
            try:
                with open(path, "r", encoding="utf-8", errors="ignore") as f:
                    lines = f.readlines()
            except Exception:
                continue
            for i, line in enumerate(lines, 1):
                for pat in patterns:
                    m = re.search(pat, line, re.IGNORECASE | re.DOTALL)
                    if m:
                        todos.append(f"- **{m.group(1)}**: {m.group(2).strip()}  (`{path}`, L{i})")

with open(todo_file, "w", encoding="utf-8") as f:
    f.write("# TODOs (auto-synced)\n\n")
    if todos:
        f.write("\n".join(todos) + "\n")
    else:
        f.write("No TODOs found. Good job!\n")
