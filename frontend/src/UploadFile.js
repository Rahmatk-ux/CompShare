import React, { useState } from "react";

export default function UploadFile() {
  const [file, setFile] = useState(null);
  const [message, setMessage] = useState("");

  const handleFileChange = (e) => setFile(e.target.files[0]);

  const handleUpload = () => {
    if (!file) return setMessage("Select a file first!");
    setMessage(`File ${file.name} uploaded & compressed!`);
  };

  return (
    <div className="card">
      <h2>Upload & Compress File</h2>
      <input type="file" onChange={handleFileChange} />
      <select defaultValue="">
        <option value="" disabled>Select file type</option>
        <option value="text">Text File</option>
        <option value="image">Image File</option>
      </select>
      <button onClick={handleUpload}>Upload & Compress</button>
      {message && <p className="message">{message}</p>}
    </div>
  );
}
