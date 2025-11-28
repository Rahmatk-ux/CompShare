import React, { useState } from "react";

export default function DownloadFile() {
  const [shareCode, setShareCode] = useState("");
  const [message, setMessage] = useState("");
  const [loading, setLoading] = useState(false);
  const [stats, setStats] = useState(null);

  // Helper function to format bytes
  const formatBytes = (bytes) => {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return Math.round(bytes / Math.pow(k, i) * 100) / 100 + ' ' + sizes[i];
  };

  // Helper function to download blob
  const downloadBlob = (blob, filename) => {
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    a.remove();
    window.URL.revokeObjectURL(url);
  };

  // Extract filename from Content-Disposition header
  const getFilename = (response, defaultName) => {
    const contentDisposition = response.headers.get("content-disposition");
    if (contentDisposition && contentDisposition.includes("filename=")) {
      return contentDisposition.split("filename=")[1].replace(/"/g, "");
    }
    return defaultName;
  };

  // Download compressed file
  const handleDownloadCompressed = async () => {
    if (!shareCode) return setMessage("Enter a share code first!");

    setLoading(true);
    setMessage("Downloading compressed file...");

    try {
      const response = await fetch(`http://127.0.0.1:5000/download/${shareCode}`);
      
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || "Download failed");
      }

      const blob = await response.blob();
      const filename = getFilename(response, "compressed_file");
      downloadBlob(blob, filename);

      setMessage("Compressed file downloaded successfully!");
    } catch (err) {
      setMessage(`Error: ${err.message}`);
    } finally {
      setLoading(false);
    }
  };

  // Decompress and download original file
  const handleDecompress = async () => {
    if (!shareCode) return setMessage("Enter a share code first!");

    setLoading(true);
    setMessage("Decompressing file...");

    try {
      const response = await fetch(`http://127.0.0.1:5000/decompress/${shareCode}`);
      
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || "Decompression failed");
      }

      const blob = await response.blob();
      const filename = getFilename(response, "decompressed_file");
      downloadBlob(blob, filename);

      setMessage("File decompressed and downloaded successfully!");
    } catch (err) {
      setMessage(`Error: ${err.message}`);
    } finally {
      setLoading(false);
    }
  };

  // Fetch compression stats
  const handleGetStats = async () => {
    if (!shareCode) return setMessage("Enter a share code first!");

    setLoading(true);
    setMessage("Fetching stats...");
    setStats(null);

    try {
      const response = await fetch(`http://127.0.0.1:5000/stats/${shareCode}`);
      
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || "Failed to fetch stats");
      }

      const data = await response.json();
      setStats(data);
      setMessage("");
    } catch (err) {
      setMessage(`Error: ${err.message}`);
      setStats(null);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="card">
      <h2>Download File</h2>
      <input
        type="text"
        placeholder="Enter Share Code"
        value={shareCode}
        onChange={(e) => setShareCode(e.target.value)}
      />
      <div style={{ display: "flex", gap: "10px", marginTop: "10px", flexWrap: "wrap" }}>
        <button onClick={handleGetStats} disabled={loading}>
          View Stats
        </button>
        <button onClick={handleDownloadCompressed} disabled={loading}>
          Download Compressed
        </button>
        <button onClick={handleDecompress} disabled={loading}>
          Decompress & Download
        </button>
      </div>
      {message && <p className="message">{message}</p>}
      
      {stats && (
        <div style={{ 
          marginTop: "20px", 
          padding: "15px", 
          backgroundColor: "#f0f0f0", 
          borderRadius: "8px",
          textAlign: "left"
        }}>
          <h3> Compression Statistics</h3>
          <p><strong>File Name:</strong> {stats.filename}</p>
          <p><strong>File Type:</strong> {stats.file_type === 'text' ? 'Text File' : 'Image File'}</p>
          <p><strong>Original Size:</strong> {formatBytes(stats.original_size)}</p>
          <p><strong>Compressed Size:</strong> {formatBytes(stats.compressed_size)}</p>
          <p><strong>Space Saved:</strong> {formatBytes(stats.space_saved)}</p>
          <p><strong>Compression Ratio:</strong> {stats.compression_ratio}</p>
          <p style={{ 
            fontSize: "1.2em", 
            color: stats.compression_percentage > 0 ? "green" : "red",
            fontWeight: "bold"
          }}>
            <strong>Compression:</strong> {stats.compression_percentage}% smaller
          </p>
        </div>
      )}
    </div>
  );
}