import React from "react";
import UploadFile from "./UploadFile";
import DownloadFile from "./DownloadFile";
import "./App.css";

function App() {
  return (
    <div className="container">
      <h1>CompShare - File Compression & Sharing</h1>
      <UploadFile />
      <DownloadFile />
    </div>
  );
}

export default App;
