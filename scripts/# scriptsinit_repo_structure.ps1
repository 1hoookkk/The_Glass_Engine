# scripts/init_repo_structure.ps1
$folders = @(
  "docs","docs/adr","docs/diagrams",
  ".github/ISSUE_TEMPLATE",".github/workflows",
  "scripts"
)
$folders | ForEach-Object { New-Item -ItemType Directory -Force -Path $_ | Out-Null }

# Create files
@"
# VisualGranularSynth

Short intro text...
"@ | Set-Content README.md

@"
# Contributing Guide

Thanks for helping! Please read this before contributing.
"@ | Set-Content CONTRIBUTING.md

@"
# Changelog

## [Unreleased]
- Initial scaffolding
"@ | Set-Content CHANGELOG.md

@"
root = true

[*]
end_of_line = lf
insert_final_newline = true
charset = utf-8
"@ | Set-Content .editorconfig

@"
// clang-format: Google style or LLVM style, tweak as needed
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 120
"@ | Set-Content .clang-format

@"
*.exe
*.dll
*.pdb
build/
out/
*.user
*.suo
*.vcxproj*
*.sln
.DS_Store
.idea/
.vscode/
"@ | Set-Content .gitignore

# Issue templates
@"
name: 🐞 Bug report
about: Something's broken
labels: bug
---

**Describe the bug**
...

**Steps to Reproduce**
1. 
2. 

**Expected behavior**
...

**Screenshots/Logs**
...

**Environment**
- OS:
- DAW:
- JUCE version:
"@ | Set-Content .github/ISSUE_TEMPLATE/bug.md

@"
name: ✨ Feature request
about: Suggest an idea
labels: feature
---

**Problem / Opportunity**
...

**Proposed solution**
...

**Alternatives considered**
...

**Extra context**
...
"@ | Set-Content .github/ISSUE_TEMPLATE/feature.md

@"
name: 🧹 Task / Chore
about: Non-feature work (refactor, docs, build)
labels: task
---

**What needs doing**
...

**Acceptance Criteria**
- [ ] Done
"@ | Set-Content .github/ISSUE_TEMPLATE/task.md

@"
## Pull Request Template

**What does this PR do?**
- 

**Related issues:** Fixes #

**How to test**
...

**Checklist**
- [ ] Build passes locally
- [ ] Docs updated
- [ ] Issue linked
"@ | Set-Content .github/pull_request_template.md

@"
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4

      - name: Setup CMake
        uses: jwlawson/actions-setup-cmake@v1

      - name: Configure
        run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

      - name: Build
        run: cmake --build build --config Release -- -m

      - name: Run Unit Tests
        run: ctest --test-dir build --output-on-failure
"@ | Set-Content .github/workflows/ci.yml

@"
# Architecture Decision Record Template

**ADR #0000: Title**
Date: YYYY-MM-DD

## Status
Proposed | Accepted | Deprecated | Superseded

## Context
What led to this decision?

## Decision
What did you decide?

## Consequences
Positive, negative, neutral consequences.
"@ | Set-Content docs/adr/0000-template.md

@"
# ADR 0001: Table-Based Mod Matrix UI
Date: 2025-07-24

## Status
Accepted

## Context
Node graphs tested slower & heavier. Table UI is faster for users.

## Decision
Use a table-based modulation matrix with drag-and-drop sources/targets.

## Consequences
+ Faster UX
- Less visually 'patch-like'
"@ | Set-Content docs/adr/0001-mod-matrix-table-ui.md

@"
# System Architecture

## High-Level Overview
- Image → ImageScanner → SpectralFrame → ModMatrix/ParamMirrors → Engines (Granular/Spectral)
- Lock-free FIFO for GUI→Audio thread data
- JUCE for plugin / DSP / UI

## Components
- core/
- engine/
- plugin/
- ui/

(diagram link here)
"@ | Set-Content docs/architecture.md

@"
# Roadmap

## Phase 1: Core Architecture (Months 1-3)
- ImageScanner + ModMatrix MVP
- Granular/Spectral basic processing
- Build system stable

## Phase 2: Advanced Features (Months 4-6)
- GPU accel, presets, brush tools
- Beta release

## Phase 3: Launch & Grow (Months 7-9)
- Marketing, docs, community
- 1.0 release

(Maintain list of issues per phase in GitHub Projects)
"@ | Set-Content docs/roadmap.md

@"
# Build Troubleshooting

## VS2022 / CMake Errors
- 'Cannot open include file': check target_sources & include dirs.
- 'Not a file VerifyGlobs.cmake': delete build/ and regenerate.

## juce_vst3_helper exit code 1
- Usually missing plugin code in target or incorrect path.

(Add more as you solve them)
"@ | Set-Content docs/build-troubleshooting.md

@"
# Mod Matrix Specification

## Goals
- Real-time safe modulation
- Source types: LFOs, Envelope Follower, Macros
- Targets: any ParamMirror

## Data Flow
(UI thread) Knob/visual values → ParamMirrors → (Audio thread) ModMatrix.processBlock()

## API
- createRoute(sourceType, ParamMirror*, amount)
- setLFOFrequency, setMacroValue, etc.

## UI
- Table with rows=Sources, columns=Targets, cells = amount
"@ | Set-Content docs/mod-matrix-spec.md

@"
#!/usr/bin/env bash
# scripts/new_adr.sh
# Usage: ./scripts/new_adr.sh "Title of decision"
i=$(printf "%04d" $(ls docs/adr | wc -l))
title=$1
filename="docs/adr/${i}-${title// /-}.md"

cat > "$filename" <<EOF
# ADR $i: $title
Date: $(date +%F)

## Status
Proposed

## Context
...

## Decision
...

## Consequences
...
EOF

echo "Created $filename"
"@ | Set-Content scripts/new_adr.sh
Set-Content scripts/new_adr.ps1 @"
param(
  [Parameter(Mandatory=$true)][string]$Title
)

$index = (Get-ChildItem docs/adr/*.md | Measure-Object).Count
$indexStr = "{0:D4}" -f $index
$filename = "docs/adr/$indexStr-$($Title -replace ' ','-').md"

@"
# ADR $indexStr: $Title
Date: $(Get-Date -Format yyyy-MM-dd)

## Status
Proposed

## Context
...

## Decision
...

## Consequences
...
"@ | Set-Content $filename

Write-Host "Created $filename"
"@
