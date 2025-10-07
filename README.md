# CompShare:	A	Multi-Role	File	Compression	and	Sharing	System	using	Huffman	Coding
### Overview

This project implements **Huffman coding-based compression and decompression** for multiple file types — **Text (.txt)**, **Image (.bmp)** **And shows stats related to Compression Ratio, bits saved, time taken** — using core data structures in C.

### Features

* Text compression using Huffman coding
* BMP image compression and decompression
* Shows Compress Ration and Time Taken 
* Modular structure with separate `.c` and `.h` files
* Works on Windows (via Makefile and MinGW)

### Data Structures Used

* **Min Heap** → Building the Huffman Tree efficiently
* **Binary Tree** → Representing Huffman tree structure
* **Queue / Array** → Managing frequencies and codes
* **Structures & Pointers** → Node and file management

### Project Structure

```
CompressionProject/
 -include/
 -src/
 -data/
 -build/
 -Makefile
 -README
 -Gitignore
```

### Build Instructions

```bash
make
make run
```

### Test Files

Add your `.txt`, `.bmp`, and `.wav` files in the `data/` folder before running.

### 🧩 Future Enhancements

* Backend Integration (Flask or Node.js) to allow uploading and sharing compressed files over a web interface.
* Database Integration (SQLite / PostgreSQL) to store file metadata, history, and sharing codes.
* Unique File Codes for secure and fast file sharing between users.
* Web Frontend using HTML, CSS, JS (React) for modern and responsive UI.
* REST API endpoints to connect backend with the core C compression engine.
