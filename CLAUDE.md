# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Unity 6 (6000.3.6f1) project implementing Gravner-Griffeath snowflake crystal growth simulations. The core algorithm models vapor diffusion, freezing, attachment, and melting on a hexagonal cellular automaton grid controlled by 9 physical parameters (ρ, β, α, θ, κ, μ, γ, σ, Δρ).

## Build & Run

This is a Unity Editor project — no CLI build commands apply. Open in Unity Hub with Unity 6000.3.6f1. Key Editor workflows:

- **Run simulation in-Editor:** Enter Play Mode with a scene containing `ObjectProperties` + `SnowflakePanel`
- **Generate snowflake mesh:** Use the custom Inspector buttons on `ObjectProperties` (calls `Operators.SnowflakeGrow`)
- **Save mesh as asset:** `Assets/Editor/SnowflakeMeshSaver.cs` → right-click selected GameObject in Editor
- **Preview mesh:** `Window > Preview Scene` (from `PreviewSceneWindow.cs`)
- **Live growth:** `ObjectProperties` hooks `EditorApplication.update` to step the simulation frame-by-frame in Edit Mode

## Architecture

### Algorithm Implementations (Three Backends)

The same Gravner-Griffeath algorithm exists in three parallel implementations — all are intentional, representing an in-progress migration from MonoBehaviour → ECS:

| Backend | Entry point | Notes |
|---|---|---|
| **Legacy MonoBehaviour** | `SnowflakeSimulator.cs` | Update-loop based, largely superseded |
| **Pure C# / async** | `Calculate.cs` + `SnowflakeCore.cs` | Uses `Parallel.For` + UniTask; primary active backend |
| **Unity ECS** | `SnowflakeSimSystem.cs` (ISystem) | Burst-friendly, still incomplete |

`Operators.cs` is the orchestration layer: `SnowflakeCreate`, `SnowflakeGrow`, `SnowflakeGrowModal`, `SnowflakeRandomize` — it coordinates whichever backend is active and handles mesh creation and USD export.

### Data Flow

```
ObjectProperties (Inspector params)
  → Operators.SnowflakeGrow()
    → Calculate.Grow() [async, Parallel.For steps]
      → Data (a/b/c/d grids on SystemData struct)
    → SimulateMesh.CreateMesh() → hexagonal mesh
  → MirrorModifier → symmetry applied
  → USD export (optional, via Modifier.cs)
```

### ECS Integration

`SnowflakeSimAuthoring` / `SnowflakeSimComponent` define the ECS data model. `SnowManagedFallSystem` bridges ECS `LocalTransform` back to MonoBehaviour GameObjects for falling-snowflake physics. `ClimateSettings` manages a concurrent spawn queue with async generation and progress bar.

### NumPy-like Utilities

Heavy use of Python-style array operations throughout:
- `NumpyClass.cs` — pure C# implementation (reshape, where, pad, cumsum, logical ops, etc.)
- `NdArray<T>` (`NdArray.cs`) — P/Invoke wrapper around a native C++ DLL (`CLanguageNative`) for performance-critical array paths
- `PythonFunction.cs`, `SliceUtils.cs` — boolean/fancy indexing, Python-style slicing

When modifying simulation math, prefer `NdArray<T>` for large grids (delegates to native) and `NumpyClass` for smaller utility operations.

### Key Parameter Container

`ObjectProperties.cs` holds all 9 simulation parameters as serialized fields with `[FloatField]` attributes (custom ±-button drawers). The `SnowFlakeParams` / `Snowflake` classes wrap these for passing into backends.

## Packages

- `com.unity.entities` 1.4.5 + `com.unity.entities.graphics` 1.4.18 — ECS stack
- `com.unity.physics` 1.4.5 — falling snowflake collisions
- `com.unity.render-pipelines.universal` 17.3.0 — URP
- `com.cysharp.unitask` (git) — async/await without blocking the main thread
- USD support (experimental) — for animation cache export

## Conventions

- Comments and internal documentation are in Japanese.
- Greek-letter variable names (ρ, β, α, etc.) match the published Gravner-Griffeath paper directly.
- Editor-only code lives under `Assets/Editor/`; runtime simulation code lives in namespace-less scripts directly under `Assets/`.
- `Memo.cs` contains developer notes and measured performance benchmarks — check it before optimizing hot paths.

## Git Push フロー

ブランチ: `dev_NumCSharp`

```
git add .
git commit -m "[メッセージ]"
git push origin dev_NumCSharp
```
