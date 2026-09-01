set shell := ["bash", "-c"]

# List all available recipes when running `just` without arguments.
default:
    @just --list

# Install the frontend and STT dependencies.
install:
    cd frontend && npm ci
    cd stt && .venv/bin/python -m pip install -e .

# Format the C++, Python, and React components.
format:
    cd backend && git ls-files -z -- '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hh' '*.hpp' '*.hxx' | xargs -0 -r clang-format --style=file -i
    cd stt && .venv/bin/python -m ruff format .
    cd frontend && npm run format

# Configure Debug, then start STT, the C++ backend, and the React development server.
run-dev-all:
    #!/usr/bin/env bash
    set -euo pipefail

    project_root="$PWD"
    pids=()

    cleanup() {
        trap - EXIT INT TERM
        if ((${#pids[@]})); then
            kill "${pids[@]}" 2>/dev/null || true
            wait "${pids[@]}" 2>/dev/null || true
        fi
    }
    trap cleanup EXIT INT TERM

    wait_for_port() {
        local service="$1" port="$2" pid="$3"
        for _ in {1..240}; do
            if (echo >/dev/tcp/127.0.0.1/"$port") >/dev/null 2>&1; then
                return 0
            fi
            if ! kill -0 "$pid" 2>/dev/null; then
                echo "$service exited before opening port $port" >&2
                wait "$pid"
                return 1
            fi
            sleep 0.5
        done
        echo "Timed out waiting for $service on port $port" >&2
        return 1
    }

    (cd "$project_root/backend" && cmake --preset debug)
    cmake --build "$project_root/backend/build"

    (cd "$project_root/stt" && exec .venv/bin/python src/main.py) &
    pids+=("$!")
    wait_for_port "STT" 8081 "${pids[-1]}"

    (cd "$project_root/backend/build" && exec ./WebPtt) &
    pids+=("$!")
    wait_for_port "backend" 8080 "${pids[-1]}"

    (cd "$project_root/frontend" && exec npm run dev) &
    pids+=("$!")

    wait -n "${pids[@]}"

# Build Release and React, then start STT, the C++ backend, and React preview.
run-release-all:
    #!/usr/bin/env bash
    set -euo pipefail

    project_root="$PWD"
    pids=()

    cleanup() {
        trap - EXIT INT TERM
        if ((${#pids[@]})); then
            kill "${pids[@]}" 2>/dev/null || true
            wait "${pids[@]}" 2>/dev/null || true
        fi
    }
    trap cleanup EXIT INT TERM

    wait_for_port() {
        local service="$1" port="$2" pid="$3"
        for _ in {1..240}; do
            if (echo >/dev/tcp/127.0.0.1/"$port") >/dev/null 2>&1; then
                return 0
            fi
            if ! kill -0 "$pid" 2>/dev/null; then
                echo "$service exited before opening port $port" >&2
                wait "$pid"
                return 1
            fi
            sleep 0.5
        done
        echo "Timed out waiting for $service on port $port" >&2
        return 1
    }

    (cd "$project_root/backend" && cmake --preset release)
    cmake --build "$project_root/backend/build"
    (cd "$project_root/frontend" && npm run build)

    (cd "$project_root/stt" && exec .venv/bin/python src/main.py) &
    pids+=("$!")
    wait_for_port "STT" 8081 "${pids[-1]}"

    (cd "$project_root/backend/build" && exec ./WebPtt) &
    pids+=("$!")
    wait_for_port "backend" 8080 "${pids[-1]}"

    (cd "$project_root/frontend" && exec npm run preview) &
    pids+=("$!")

    wait -n "${pids[@]}"
