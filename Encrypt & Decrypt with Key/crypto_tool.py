import tkinter as tk
from tkinter import filedialog, messagebox
import os

# --- CONFIG ---
KEY_SIZE = 256
KEY_FILENAME = "key.dat"
MAGIC_HEADER = b"ENC1"
CHUNK_SIZE = 64 * 1024  # 64 KB chunk size for streaming
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# --- Key creation ---


def create_key():
    try:
        # os.urandom gives uniformly random bytes 0-255
        key_bytes = os.urandom(KEY_SIZE)
        with open(os.path.join(SCRIPT_DIR, KEY_FILENAME), "wb") as f:
            f.write(key_bytes)
        messagebox.showinfo(
            "Key Created", f"{KEY_FILENAME} has been created ({KEY_SIZE} bytes).")
    except Exception as e:
        messagebox.showerror("Error", f"Failed to create key: {e}")

# --- Encrypt (streamed) ---


def encrypt_file():
    input_path = filedialog.askopenfilename(title="Select File to Encrypt")
    if not input_path:
        return

    if not os.path.exists(KEY_FILENAME):
        messagebox.showerror(
            "Key Missing", f"{KEY_FILENAME} not found. Please create it first.")
        return

    try:
        with open(os.path.join(SCRIPT_DIR, KEY_FILENAME), "rb") as f:
            key = f.read()
    except Exception as e:
        messagebox.showerror("Error", f"Failed to read key: {e}")
        return

    if len(key) != KEY_SIZE:
        messagebox.showerror(
            "Invalid Key", f"The key file must be exactly {KEY_SIZE} bytes.")
        return

    try:
        with open(input_path, "rb") as fin:
            # quick check whether file already encrypted (read first 4 bytes)
            start = fin.read(4)
            if start == MAGIC_HEADER:
                messagebox.showwarning(
                    "Already Encrypted", "This file is already encrypted.")
                return
            # rewind to beginning for full streaming
            fin.seek(0)

            output_path = os.path.join(
                SCRIPT_DIR, os.path.basename(input_path) + ".enc")
            with open(output_path, "wb") as fout:
                fout.write(MAGIC_HEADER)  # write header

                pos = 0  # position in file (used to index key)
                while True:
                    chunk = fin.read(CHUNK_SIZE)
                    if not chunk:
                        break
                    out = bytearray(len(chunk))
                    # apply key (key is bytes-like)
                    for i, b in enumerate(chunk):
                        out[i] = (b + key[(pos + i) % KEY_SIZE]) % 256
                    fout.write(out)
                    pos += len(chunk)

        messagebox.showinfo("Encryption Complete",
                            f"Encrypted to: {output_path}")

    except Exception as e:
        messagebox.showerror("Error", f"Encryption failed: {e}")

# --- Decrypt (streamed) ---


def decrypt_file():
    input_path = filedialog.askopenfilename(
        title="Select Encrypted (.enc) File", filetypes=[("Encrypted files", "*.enc")])
    if not input_path:
        return

    if not os.path.exists(KEY_FILENAME):
        messagebox.showerror(
            "Key Missing", f"{KEY_FILENAME} not found. Please create it first.")
        return

    try:
        with open(os.path.join(SCRIPT_DIR, KEY_FILENAME), "rb") as f:
            key = f.read()
    except Exception as e:
        messagebox.showerror("Error", f"Failed to read key: {e}")
        return

    if len(key) != KEY_SIZE:
        messagebox.showerror(
            "Invalid Key", f"The key file must be exactly {KEY_SIZE} bytes.")
        return

    try:
        with open(input_path, "rb") as fin:
            header = fin.read(4)
            if header != MAGIC_HEADER:
                messagebox.showwarning(
                    "Not Encrypted", "This file does not have a valid encryption header.")
                return

            original_name = os.path.basename(input_path).replace(".enc", "")
            output_path = os.path.join(SCRIPT_DIR, original_name)
            
            with open(output_path, "wb") as fout:
                pos = 0
                while True:
                    chunk = fin.read(CHUNK_SIZE)
                    if not chunk:
                        break
                    out = bytearray(len(chunk))
                    for i, b in enumerate(chunk):
                        out[i] = (b - key[(pos + i) % KEY_SIZE] + 256) % 256
                    fout.write(out)
                    pos += len(chunk)

        messagebox.showinfo("Decryption Complete",
                            f"Decrypted to: {output_path}")

    except Exception as e:
        messagebox.showerror("Error", f"Decryption failed: {e}")


# --- GUI Setup ---
app = tk.Tk()
app.title("🔐 Secure File Crypto Tool")
app.geometry("480x420")
app.resizable(False, False)

tk.Label(app, text="🔐 Secure File Crypto Tool", font=(
    "Segoe UI", 18, "bold"), fg="#004d40").pack(pady=20)

tk.Button(app, text="🔑 Generate Key (key.dat)", command=create_key,
          bg="#26a69a", fg="white", font=("Segoe UI", 12), width=36).pack(pady=10)
tk.Button(app, text="🔒 Encrypt a File", command=encrypt_file, bg="#00796b",
          fg="white", font=("Segoe UI", 12), width=36).pack(pady=10)
tk.Button(app, text="🔓 Decrypt a File", command=decrypt_file, bg="#388e3c",
          fg="white", font=("Segoe UI", 12), width=36).pack(pady=10)

tk.Label(app, text=f"\nNote: The same key.dat ({KEY_SIZE} bytes) must be used to decrypt.", font=(
    "Segoe UI", 10), fg="#777").pack()

app.mainloop()
