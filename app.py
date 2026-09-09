from flask import *
import subprocess
import os
import platform

app = Flask(__name__)

HUFFMAN_EXECUTABLE = (
    "huffman.exe"
    if platform.system() == "Windows"
    else "./huffman"
)

UPLOAD = "uploads"
OUTPUT = "output"

os.makedirs(UPLOAD, exist_ok=True)
os.makedirs(OUTPUT, exist_ok=True)


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/compress", methods=["POST"])
def compress():

    file = request.files["file"]

    in_path = os.path.join(UPLOAD, file.filename)
    out_path = os.path.join(OUTPUT, file.filename + ".huff")

    file.save(in_path)

    # WINDOWS FIX (use .exe)
    subprocess.run([
        HUFFMAN_EXECUTABLE,
        "compress",
        in_path,
        out_path
    ], check=True)

    return send_file(out_path, as_attachment=True)


@app.route("/decompress", methods=["POST"])
def decompress():

    file = request.files["file"]

    in_path = os.path.join(UPLOAD, file.filename)
    out_path = os.path.join(OUTPUT, "decoded_" + file.filename)

    file.save(in_path)

    subprocess.run([
    HUFFMAN_EXECUTABLE,
    "decompress",
    in_path,
    out_path
], check=True)

    return send_file(out_path, as_attachment=True)


if __name__ == "__main__":
    app.run(debug=True)
