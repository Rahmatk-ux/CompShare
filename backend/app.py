from flask import Flask, request, jsonify, send_file
import os
import sqlite3
import subprocess
import string, random
from werkzeug.utils import secure_filename

app = Flask(__name__)
UPLOAD_FOLDER = '../data'           # Your existing data folder
COMPRESSED_FOLDER = './compressed' # Backend compressed files folder
os.makedirs(COMPRESSED_FOLDER, exist_ok=True)

# Initialize SQLite database
def init_db():
    conn = sqlite3.connect('db.sqlite')
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

# Generate unique share code
def generate_code(length=6):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

# Upload endpoint
@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({"message": "No file part"}), 400
    file = request.files['file']
    if file.filename == '':
        return jsonify({"message": "No selected file"}), 400

    filename = secure_filename(file.filename)
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    file.save(filepath)
    return jsonify({"message": "File uploaded", "filepath": filepath})

# Compress endpoint
@app.route('/compress', methods=['POST'])
def compress_file():
    data = request.get_json()
    input_file = data['filepath']
    file_type = data['file_type']  # 'text' or 'image'

    filename = os.path.basename(input_file)
    compressed_file = os.path.join(COMPRESSED_FOLDER, 'compressed_' + filename)

    # Call your C executable
    c_executable = './CompShare'  # Ensure it's copied from src/ to backend/
    if file_type == 'text':
        subprocess.run([c_executable, 'compress_txt', input_file, compressed_file])
    elif file_type == 'image':
        subprocess.run([c_executable, 'compress_img', input_file, compressed_file])
    else:
        return jsonify({"message": "Invalid file type"}), 400

    # Compute compression ratio
    original_size = os.path.getsize(input_file)
    compressed_size = os.path.getsize(compressed_file)
    ratio = compressed_size / original_size

    # Store in SQLite database
    share_code = generate_code()
    conn = sqlite3.connect('db.sqlite')
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

# Decompress endpoint
@app.route('/decompress', methods=['POST'])
def decompress_file():
    data = request.get_json()
    input_file = data['filepath']
    file_type = data['file_type']  # 'text' or 'image'

    filename = os.path.basename(input_file)
    decompressed_file = os.path.join(COMPRESSED_FOLDER, 'decompressed_' + filename)

    # Call your C executable
    c_executable = './CompShare'
    if file_type == 'text':
        subprocess.run([c_executable, 'decompress_txt', input_file, decompressed_file])
    elif file_type == 'image':
        subprocess.run([c_executable, 'decompress_img', input_file, decompressed_file])
    else:
        return jsonify({"message": "Invalid file type"}), 400

    return jsonify({
        "message": "File decompressed",
        "decompressed_file": decompressed_file
    })

# Download by share code
@app.route('/share/<code>', methods=['GET'])
def share_file(code):
    conn = sqlite3.connect('db.sqlite')
    cursor = conn.cursor()
    cursor.execute('SELECT compressed_path FROM files WHERE share_code=?', (code,))
    row = cursor.fetchone()
    conn.close()
    if row:
        return send_file(row[0], as_attachment=True)
    return jsonify({"message": "Invalid code"}), 404

if __name__ == '__main__':
    app.run(debug=True)
