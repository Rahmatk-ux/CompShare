# CompShare — Full-Stack File Compression and Sharing Platform

A full-stack file compression tool built around a native C compression engine. A React frontend and Flask API handle uploads, sharing, and stats, while the actual Huffman coding compression runs in an isolated C binary invoked via subprocess.

## Overview

CompShare lets a user upload a text or image file, compresses it using Huffman coding implemented from scratch in C, and generates a unique share code that can be used to download the compressed file or decompress it back to the original. File metadata and compression stats are tracked in SQLite.

## Architecture

```
React Frontend  →  Flask REST API  →  subprocess  →  Native C Engine (Huffman Coding)
                         ↓
                     SQLite (file metadata, share codes, compression stats)
```

The C engine is fully decoupled from the web layer. Flask calls it as a subprocess, so a crash or bad input in the native binary cannot bring down the API server.

## Features

- Huffman coding compression/decompression for text and image (BMP) files, implemented in C with a custom Min-Heap
- React UI for uploading files and viewing live compression stats
- Flask REST API with endpoints for upload, compress, decompress, download, and stats
- SQLite-backed share codes — each compressed file gets a unique 6-character alphanumeric code for retrieval
- Real compression results on test data: 911.79 KB → 406.64 KB (55.4% size reduction)

## Data Structures Used

- **Min-Heap** — builds the Huffman tree by repeatedly merging lowest-frequency nodes
- **Binary Tree** — represents the Huffman encoding tree
- **Hash structures / arrays** — track character frequencies before tree construction
- **SQLite tables** — map share codes to file paths, types, and compression ratios

## C Module Breakdown

| File | Responsibility |
|---|---|
| `huffman.c` | Huffman tree construction and Min-Heap logic |
| `txt.c` | Text file compression/decompression |
| `img.c` | BMP image compression/decompression |
| `stats.c` | Compression ratio and size statistics |
| `main.c` | CLI entry point — dispatches `compress_txt`, `compress_img`, `decompress_txt`, `decompress_img` commands, called by Flask via subprocess |

## Tech Stack

| Layer | Technology |
|---|---|
| Compression engine | C |
| Backend API | Python (Flask) |
| Frontend | React |
| Storage | SQLite |
| Process isolation | Python `subprocess` |

## Project Structure

```
CompShare/
├── backend/
│   ├── app.py            # Flask API (upload, compress, decompress, stats)
│   ├── CompShare.exe      # Compiled C engine, used by Flask via subprocess
│   ├── db.sqlite          # File metadata and share codes
│   ├── compressed/        # Compressed/decompressed output files
│   └── data/
├── frontend/
│   ├── src/
│   │   ├── App.js
│   │   ├── UploadFile.js  # Upload + compress UI
│   │   └── DownloadFile.js # Download/decompress/stats UI
│   └── public/
├── include/                # C headers
│   ├── huffman.h
│   ├── img.h
│   ├── stats.h
│   └── txt.h
├── src/                     # C source (compression engine)
│   ├── huffman.c            # Huffman tree + Min-Heap implementation
│   ├── img.c                # BMP image compression/decompression
│   ├── main.c                # CLI entry point used by subprocess calls
│   ├── stats.c                # Compression ratio / size stats
│   └── txt.c                 # Text file compression/decompression
├── data/                    # Sample input files for testing
├── Makefile
└── README.md
```

## How It Works

1. User uploads a file through the React frontend.
2. Flask saves the file and calls the C binary via `subprocess` to compress it (`compress_txt` or `compress_img` depending on type).
3. Flask computes the compression ratio from the resulting file sizes and stores the result in SQLite under a freshly generated share code.
4. The user can later use the share code to view stats, download the compressed file, or trigger decompression back to the original file via the same C binary.

## Run Locally

**Backend**
```bash
cd backend
pip install flask flask-cors
python app.py
```

**Frontend**
```bash
cd frontend
npm install
npm start
```

**C engine**
```bash
mingw32-make
```
This compiles `src/main.c`, `src/huffman.c`, `src/txt.c`, `src/img.c`, and `src/stats.c` into `CompShare.exe`, then automatically copies it into `backend/` so Flask's `subprocess` calls can find it. Run `mingw32-make clean` to remove build artifacts.

> Built and tested on Windows with MinGW. The Makefile uses Windows-specific commands (`copy`, `del /Q`), so it will need adjusting (`cp`, `rm`) to run on Linux/Mac.
