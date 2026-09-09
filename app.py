from flask import *
import subprocess, os

app = Flask(__name__)

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
        "huffman.exe",
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
        "huffman.exe",
        "decompress",
        in_path,
        out_path
    ], check=True)

    return send_file(out_path, as_attachment=True)


if __name__ == "__main__":
    app.run(debug=True)
