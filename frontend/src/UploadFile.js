import React, { useState } from "react";

export default function UploadFile() {
  const [file, setFile] = useState(null);
  const [fileType, setFileType] = useState("");
  const [message, setMessage] = useState("");
  const [compressionData, setCompressionData] = useState(null);

  const handleFileChange = (e) => {
    setFile(e.target.files[0]);
    setCompressionData(null);
  };
  
  const handleTypeChange = (e) => setFileType(e.target.value);

  // Helper function to format bytes
  const formatBytes = (bytes) => {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return Math.round(bytes / Math.pow(k, i) * 100) / 100 + ' ' + sizes[i];
  };

  const handleUpload = async () => {
    if (!file) return setMessage("Select a file first!");
    if (!fileType) return setMessage("Select a file type!");

    const formData = new FormData();
    formData.append("file", file);

    try {
      // 1️⃣ Upload file to Flask
      const uploadRes = await fetch("http://127.0.0.1:5000/upload", {
        method: "POST",
        body: formData
      });
      const uploadData = await uploadRes.json();
      if (!uploadRes.ok) throw new Error(uploadData.message || "Upload failed");

      const filepath = uploadData.filepath;

      // 2️⃣ Compress file via Flask
      const compressRes = await fetch("http://127.0.0.1:5000/compress", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ filepath, file_type: fileType })
      });
      const compressData = await compressRes.json();
      if (!compressRes.ok) throw new Error(compressData.message || "Compression failed");

      // 3️⃣ Fetch detailed stats
      const statsRes = await fetch(`http://127.0.0.1:5000/stats/${compressData.share_code}`);
      const statsData = await statsRes.json();
      
      if (statsRes.ok) {
        setCompressionData(statsData);
      }

      setMessage(
        `File uploaded & compressed successfully! Share code: ${compressData.share_code}`
      );
    } catch (err) {
      setMessage(`Error: ${err.message}`);
      setCompressionData(null);
    }
  };

  return (
    <div className="card">
      <h2>Upload & Compress File</h2>
      <input type="file" onChange={handleFileChange} />
      <select value={fileType} onChange={handleTypeChange}>
        <option value="" disabled>Select file type</option>
        <option value="text">Text File</option>
        <option value="image">Image File</option>
      </select>
      <button onClick={handleUpload}>Upload & Compress</button>
      {message && <p className="message">{message}</p>}

      {compressionData && (
        <div style={{ 
          marginTop: "20px", 
          padding: "15px", 
          backgroundColor: "#e8f5e9", 
          borderRadius: "8px",
          textAlign: "left"
        }}>
          <h3> Compression Successful!</h3>
          <p><strong>Share Code:</strong> <span style={{fontSize: "1.2em", color: "#1976d2"}}>{compressionData.share_code}</span></p>
          <p><strong>File Name:</strong> {compressionData.filename}</p>
          <p><strong>Original Size:</strong> {formatBytes(compressionData.original_size)}</p>
          <p><strong>Compressed Size:</strong> {formatBytes(compressionData.compressed_size)}</p>
          <p><strong>Space Saved:</strong> {formatBytes(compressionData.space_saved)}</p>
          <p style={{ 
            fontSize: "1.3em", 
            color: "green",
            fontWeight: "bold",
            marginTop: "10px"
          }}>
             {compressionData.compression_percentage}% size reduction!
          </p>
        </div>
      )}
    </div>
  );
}