from flask import Flask, request, jsonify, send_file
import os
import sqlite3
import subprocess
import string, random
import sys
from werkzeug.utils import secure_filename
from flask_cors import CORS

# --- Initialize Flask and enable CORS ---
app = Flask(__name__)
CORS(app)  # allow requests from React frontend

# --- Define folders ---
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
UPLOAD_FOLDER = os.path.join(BASE_DIR, '../data')
COMPRESSED_FOLDER = os.path.join(BASE_DIR, 'compressed')

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(COMPRESSED_FOLDER, exist_ok=True)

# --- SQLite DB initialization ---
DB_PATH = os.path.join(BASE_DIR, 'db.sqlite')

def init_db():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS files (
            id INTEGER PRIMARY KEY,
            filename TEXT,
            filepath TEXT,
            compressed_path TEXT,
            file_type TEXT,
            compression_ratio REAL,
            share_code TEXT UNIQUE
        )
    ''')
    conn.commit()
    conn.close()

init_db()

# --- Generate unique share code ---
def generate_code(length=6):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

# --- Detect C executable for Windows/Linux ---
if sys.platform.startswith("win"):
    C_EXECUTABLE = os.path.join(BASE_DIR, "CompShare.exe")
else:
    C_EXECUTABLE = os.path.join(BASE_DIR, "CompShare")

if not os.path.exists(C_EXECUTABLE):
    print(f"Error: C executable not found at {C_EXECUTABLE}")

# --- Upload Endpoint ---
@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({"message": "No file part"}), 400

    file = request.files['file']
    if file.filename == '':
        return jsonify({"message": "No selected file"}), 400

    filename = secure_filename(file.filename)
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    # Always write uploads in binary
    with open(filepath, "wb") as f:
        f.write(file.read())

    return jsonify({"message": "File uploaded", "filepath": filepath})


# --- Compress Endpoint ---
@app.route('/compress', methods=['POST'])
def compress_file():
    data = request.get_json()
    input_file = data.get('filepath')
    file_type = data.get('file_type')

    if not input_file or not file_type:
        return jsonify({"message": "Missing parameters"}), 400
    if not os.path.exists(C_EXECUTABLE):
        return jsonify({"message": "C executable not found"}), 500

    filename = os.path.basename(input_file)
    compressed_file = os.path.join(COMPRESSED_FOLDER, 'compressed_' + filename)

    # Build command
    if file_type == 'text':
        cmd = [C_EXECUTABLE, 'compress_txt', input_file, compressed_file]
    elif file_type == 'image':
        cmd = [C_EXECUTABLE, 'compress_img', input_file, compressed_file]
    else:
        return jsonify({"message": "Invalid file type"}), 400

    # Run C program
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        error_msg = result.stderr.decode() if result.stderr else "Unknown error"
        return jsonify({"message": "Compression failed", "error": error_msg}), 500

    # Compute compression ratio
    original_size = os.path.getsize(input_file)
    compressed_size = os.path.getsize(compressed_file)
    ratio = compressed_size / original_size if original_size else 0

    # Store in DB
    share_code = generate_code()
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute(
        'INSERT INTO files (filename, filepath, compressed_path, file_type, compression_ratio, share_code) VALUES (?,?,?,?,?,?)',
        (filename, input_file, compressed_file, file_type, ratio, share_code)
    )
    conn.commit()
    conn.close()

    return jsonify({
        "message": "File compressed",
        "compressed_file": compressed_file,
        "share_code": share_code,
        "ratio": ratio
    })

# --- Decompress Endpoint (FIXED - Now sends file back) ---
@app.route('/decompress/<code>', methods=['GET'])
def decompress_file(code):
    # Fetch file info from database
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('SELECT compressed_path, file_type, filename FROM files WHERE share_code=?', (code,))
    row = cursor.fetchone()
    conn.close()

    if not row:
        return jsonify({"message": "Invalid share code"}), 404

    compressed_path, file_type, original_filename = row
    
    if not os.path.exists(compressed_path):
        return jsonify({"message": "Compressed file not found"}), 404

    if not os.path.exists(C_EXECUTABLE):
        return jsonify({"message": "C executable not found"}), 500

    # Create decompressed filename
    decompressed_file = os.path.join(COMPRESSED_FOLDER, 'decompressed_' + original_filename)

    # Build command
    if file_type == 'text':
        cmd = [C_EXECUTABLE, 'decompress_txt', compressed_path, decompressed_file]
        mimetype = 'text/plain'
    elif file_type == 'image':
        cmd = [C_EXECUTABLE, 'decompress_img', compressed_path, decompressed_file]
        mimetype = 'image/bmp'
    else:
        return jsonify({"message": "Invalid file type"}), 400

    # Run C program
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        error_msg = result.stderr.decode() if result.stderr else "Unknown error"
        return jsonify({"message": "Decompression failed", "error": error_msg}), 500

    # Verify decompressed file was created
    if not os.path.exists(decompressed_file):
        return jsonify({"message": "Decompressed file was not created"}), 500

    # Send the actual file back to frontend
    return send_file(
        decompressed_file,
        as_attachment=True,
        download_name=original_filename,
        mimetype=mimetype
    )

# --- Download Compressed File Endpoint ---
@app.route('/download/<code>', methods=['GET'])
def download_compressed(code):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('SELECT compressed_path, filename FROM files WHERE share_code=?', (code,))
    row = cursor.fetchone()
    conn.close()

    if not row:
        return jsonify({"message": "Invalid code"}), 404
    
    compressed_path, original_filename = row
    
    if not os.path.exists(compressed_path):
        return jsonify({"message": "File missing"}), 404

    return send_file(
        compressed_path,
        as_attachment=True,
        download_name=f'compressed_{original_filename}',
        mimetype="application/octet-stream"
    )

# --- Legacy share endpoint (kept for compatibility) ---
@app.route('/share/<code>', methods=['GET'])
def share_file(code):
    return download_compressed(code)

# --- Get File Stats Endpoint ---
@app.route('/stats/<code>', methods=['GET'])
def get_stats(code):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        SELECT filename, file_type, compression_ratio, filepath, compressed_path 
        FROM files WHERE share_code=?
    ''', (code,))
    row = cursor.fetchone()
    conn.close()

    if not row:
        return jsonify({"message": "Invalid share code"}), 404

    filename, file_type, compression_ratio, original_path, compressed_path = row

    # Get file sizes
    original_size = os.path.getsize(original_path) if os.path.exists(original_path) else 0
    compressed_size = os.path.getsize(compressed_path) if os.path.exists(compressed_path) else 0

    # Calculate stats
    space_saved = original_size - compressed_size
    compression_percentage = (1 - compression_ratio) * 100 if compression_ratio else 0

    return jsonify({
        "filename": filename,
        "file_type": file_type,
        "original_size": original_size,
        "compressed_size": compressed_size,
        "compression_ratio": round(compression_ratio, 4),
        "space_saved": space_saved,
        "compression_percentage": round(compression_percentage, 2),
        "share_code": code
    })

# --- Run Flask App ---
if __name__ == '__main__':
    app.run(debug=True)