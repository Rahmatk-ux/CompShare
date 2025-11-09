import React, { useState } from "react";

export default function DownloadFile() {
  const [shareCode, setShareCode] = useState("");
  const [message, setMessage] = useState("");

  const handleDownload = () => {
    if (!shareCode) return setMessage("Enter a share code first!");
    setMessage(`Downloading file with code: ${shareCode}`);
  };

  return (
    <div className="card">
      <h2>Download Shared File</h2>
      <input
        type="text"
        placeholder="Enter Share Code"
        value={shareCode}
        onChange={(e) => setShareCode(e.target.value)}
      />
      <button onClick={handleDownload}>Download</button>
      {message && <p className="message">{message}</p>}
    </div>
  );
}
