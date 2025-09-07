import tkinter as tk
from tkinter import filedialog, messagebox
import os
import random

KEY_SIZE = 256
KEY_FILENAME = "key.dat"
MAGIC_HEADER = b"ENC1"

def create_key():
    key = list(range(KEY_SIZE))
    random.shuffle(key)
    with open(KEY_FILENAME, "wb") as f:
        f.write(bytearray(key))
    messagebox.showinfo("Key Created", f"{KEY_FILENAME} has been created.")

def encrypt_file():
    input_path = filedialog.askopenfilename(title="Select File to Encrypt")
    if not input_path:
        return

    if not os.path.exists(KEY_FILENAME):
        messagebox.showerror("Key Missing", f"{KEY_FILENAME} not found. Please create it first.")
        return

    with open(KEY_FILENAME, "rb") as f:
        key = f.read(KEY_SIZE)
        if len(key) != KEY_SIZE:
            messagebox.showerror("Invalid Key", "The key file must be 256 bytes.")
            return

    with open(input_path, "rb") as f:
        data = f.read()

    if data.startswith(MAGIC_HEADER):
        messagebox.showwarning("Already Encrypted", "This file is already encrypted.")
        return

    encrypted = bytearray(MAGIC_HEADER)
    for i in range(len(data)):
        encrypted.append((data[i] + key[i % KEY_SIZE]) % 256)

    output_path = input_path + ".enc"
    with open(output_path, "wb") as f:
        f.write(encrypted)

    os.remove(input_path)
    messagebox.showinfo("Encryption Complete", f"Encrypted to: {output_path}")

def decrypt_file():
    input_path = filedialog.askopenfilename(title="Select Encrypted (.enc) File", filetypes=[("Encrypted files", "*.enc")])
    if not input_path:
        return

    if not os.path.exists(KEY_FILENAME):
        messagebox.showerror("Key Missing", f"{KEY_FILENAME} not found. Please create it first.")
        return

    with open(KEY_FILENAME, "rb") as f:
        key = f.read(KEY_SIZE)
        if len(key) != KEY_SIZE:
            messagebox.showerror("Invalid Key", "The key file must be 256 bytes.")
            return

    with open(input_path, "rb") as f:
        data = f.read()

    if not data.startswith(MAGIC_HEADER):
        messagebox.showwarning("Not Encrypted", "This file does not have a valid encryption header.")
        return

    decrypted = bytearray()
    for i in range(4, len(data)):
        decrypted.append((data[i] - key[(i - 4) % KEY_SIZE] + 256) % 256)

    output_path = input_path.replace(".enc", "")
    with open(output_path, "wb") as f:
        f.write(decrypted)

    os.remove(input_path)
    messagebox.showinfo("Decryption Complete", f"Decrypted to: {output_path}")

# --- GUI Setup ---

app = tk.Tk()
app.title("🔐 Secure File Crypto Tool")
app.geometry("450x400")
app.resizable(False, False)

tk.Label(app, text="🔐 Secure File Crypto Tool", font=("Segoe UI", 18, "bold"), fg="#004d40").pack(pady=20)

tk.Button(app, text="🔑 Generate Key (key.dat)", command=create_key, bg="#26a69a", fg="white", font=("Segoe UI", 12), width=30).pack(pady=10)
tk.Button(app, text="🔒 Encrypt a File", command=encrypt_file, bg="#00796b", fg="white", font=("Segoe UI", 12), width=30).pack(pady=10)
tk.Button(app, text="🔓 Decrypt a File", command=decrypt_file, bg="#388e3c", fg="white", font=("Segoe UI", 12), width=30).pack(pady=10)

tk.Label(app, text="\nNote: The same key.dat must be used to decrypt.", font=("Segoe UI", 10), fg="#777").pack()

app.mainloop()
