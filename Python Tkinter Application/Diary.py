import os
import sys
import struct
import random
import time
import shutil

import tkinter as tk
from tkinter import simpledialog, messagebox, scrolledtext, ttk

ENCRYPTION_ARRAY_SIZE = 256
MAX_USERNAME_LEN = 20
MAX_PASSWORD_LEN = 20
BACKUP_DIR = "Backup"
SETTINGS_FILE = "settings.dat"
GLOBAL_CODE_FILE = "Encryption Code.dat"

# ---------- Low-level binary helpers ----------


def read_short_array_256(path):
    with open(path, 'rb') as f:
        data = f.read()
    if len(data) >= 2 * ENCRYPTION_ARRAY_SIZE:
        fmt = '<' + ('h' * ENCRYPTION_ARRAY_SIZE)
        if len(data) < struct.calcsize(fmt):
            raise ValueError("Encryption Code.dat too small")
        arr = list(struct.unpack(fmt, data[:struct.calcsize(fmt)]))
        arr = [int(x) & 0xFF for x in arr]
        return arr
    raise ValueError("Invalid Encryption Code.dat")


def encrypt_file_in_place(path, direction, code):
    if not os.path.exists(path):
        return
    with open(path, 'rb+') as f:
        idx = 0
        while True:
            chunk = f.read(65536)
            if not chunk:
                break
            out = bytearray(len(chunk))
            for i, b in enumerate(chunk):
                out[i] = (b + direction * code[idx %
                          ENCRYPTION_ARRAY_SIZE]) & 0xFF
                idx += 1
            f.seek(-len(chunk), os.SEEK_CUR)
            f.write(out)


def ensure_backup_dir():
    os.makedirs(BACKUP_DIR, exist_ok=True)


def backup_file(src_path, code, make=True):
    ensure_backup_dir()
    dst_path = os.path.join(BACKUP_DIR, os.path.basename(src_path))
    if make:
        shutil.copy2(src_path, dst_path)
        encrypt_file_in_place(dst_path, +1, code)
    else:
        try:
            os.remove(dst_path)
        except FileNotFoundError:
            pass

# ---------- Settings management ----------


def load_global_code_or_exit():
    if not os.path.exists(GLOBAL_CODE_FILE):
        messagebox.showerror(
            "Error", "Missing 'Encryption Code.dat'. Exiting.")
        sys.exit(0)
    return read_short_array_256(GLOBAL_CODE_FILE)


def decrypt_settings_if_exists(global_code):
    if os.path.exists(SETTINGS_FILE):
        encrypt_file_in_place(SETTINGS_FILE, -1, global_code)


def encrypt_settings(global_code):
    if os.path.exists(SETTINGS_FILE):
        encrypt_file_in_place(SETTINGS_FILE, +1, global_code)


def init_settings(global_code):
    decrypt_settings_if_exists(global_code)
    if not os.path.exists(SETTINGS_FILE):
        with open(SETTINGS_FILE, 'wb') as f:
            f.write(struct.pack('<i', 0))
            f.write(b'G0')  # default: bg = '#efefef', fg = black
    backup_file(SETTINGS_FILE, global_code, make=True)


def get_all_user_records():
    with open(SETTINGS_FILE, 'rb') as f:
        no_of_user = struct.unpack('<i', f.read(4))[0]
        color = f.read(2)  # 2-char color code like b'07'
        records = []
        for _ in range(no_of_user):
            raw = f.read(4 + 2 * ENCRYPTION_ARRAY_SIZE)
            if len(raw) < 4 + 2 * ENCRYPTION_ARRAY_SIZE:
                raise ValueError("Corrupt settings.dat")
            uid = struct.unpack('<i', raw[:4])[0]
            vals = list(struct.unpack('<' + 'h'*ENCRYPTION_ARRAY_SIZE,
                                      raw[4:4+2*ENCRYPTION_ARRAY_SIZE]))
            vals = [v & 0xFF for v in vals]
            records.append((uid, vals))
        return no_of_user, color, records


def write_all_user_records(no_of_user, color, records):
    with open(SETTINGS_FILE, 'wb') as f:
        f.write(struct.pack('<i', int(no_of_user)))
        if not isinstance(color, (bytes, bytearray)) or len(color) != 2:
            color = b'07'
        f.write(color)
        for uid, code in records:
            f.write(struct.pack('<i', int(uid)))
            packed = struct.pack(
                '<' + 'h'*ENCRYPTION_ARRAY_SIZE, *[c & 0xFF for c in code])
            f.write(packed)


def generate_unique_id(existing_ids):
    while True:
        v = random.randint(10000, 99999)
        if v not in existing_ids:
            return v


def generate_encryption_code():
    items = list(range(256))
    code = []
    for _ in range(256):
        r = random.randrange(0, len(items))
        code.append(items.pop(r))
    return code

# ---------- Diary file (per user) ----------


def pad_fixed(s, maxlen=21):
    b = s.encode('utf-8')[:maxlen-1]
    b = b + b'\x00'*(maxlen - len(b))
    return b


def read_diary(path):
    with open(path, 'rb') as f:
        username = f.read(21).split(b'\x00', 1)[
            0].decode('utf-8', errors='ignore')
        password = f.read(21).split(b'\x00', 1)[
            0].decode('utf-8', errors='ignore')
        amt = struct.unpack('<i', f.read(4))[0]
        notes = []
        for _ in range(amt):
            tlen = struct.unpack('<i', f.read(4))[0]
            title = f.read(tlen).decode('utf-8', errors='ignore')
            xlen = struct.unpack('<i', f.read(4))[0]
            text = f.read(xlen).decode('utf-8', errors='ignore')
            notes.append((title, text))
        return username, password, notes


def write_diary(path, username, password, notes):
    with open(path, 'wb') as f:
        f.write(pad_fixed(username, 21))
        f.write(pad_fixed(password, 21))
        f.write(struct.pack('<i', len(notes)))
        for title, text in notes:
            tb = title.encode('utf-8')
            xb = text.encode('utf-8')
            f.write(struct.pack('<i', len(tb)))
            f.write(tb)
            f.write(struct.pack('<i', len(xb)))
            f.write(xb)

# ---------- Tkinter GUI ----------


class DiaryApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Encrypted Diary")
        self.global_code = load_global_code_or_exit()
        init_settings(self.global_code)

        self.current_file = None
        self.current_code = None
        self.username = None
        self.password = None
        self.notes = []

        # load colors from settings
        _, color, _ = get_all_user_records()
        self.bg_color, self.fg_color, self.bg_code, self.fg_code = self.decode_colors(
            color or b'G0')
        self.root.configure(bg=self.bg_color)

        # a common container frame
        self.page = tk.Frame(self.root, bg=self.bg_color)
        self.page.pack(fill="none", expand=True, padx=10, pady=10)

        self.apply_theme_to_widgets()
        self.show_main_menu()

    # -------- color helpers --------
    def decode_colors(self, code_bytes):
        """Convert 2-byte code (e.g., b'G0') → tkinter (bg, fg) and keep codes."""
        # Fallback to our default pair if missing/invalid
        if not code_bytes or len(code_bytes) != 2:
            return "#efefef", "black", "G", "0"

        bg_code = code_bytes[:1].decode(errors="ignore") or "G"
        fg_code = code_bytes[1:2].decode(errors="ignore") or "0"

        # Map single-char console-like codes to Tk colors.
        color_map = {
            "0": "black",
            "1": "blue",
            "2": "green",
            "3": "aqua",
            "4": "red",
            "5": "purple",
            "6": "yellow",
            "7": "white",
            "8": "gray",
            "9": "lightblue",
            # Custom extension:
            "G": "#efefef",  # Default Light Gray background for our GUI
        }

        bg = color_map.get(bg_code, "#efefef")  # default bg
        fg = color_map.get(fg_code, "black")    # default fg
        return bg, fg, bg_code, fg_code

    # -------- shared helpers --------

    def clear(self):
        for w in self.page.winfo_children():
            w.destroy()

    def build_right_table(self, parent):
        """Treeview with S.No. and Title; returns (frame, tree)"""
        right = tk.Frame(parent, bg=self.bg_color)
        right.grid_columnconfigure(0, weight=1)
        cols = ("sno", "title")
        tree = ttk.Treeview(right, columns=cols, show="headings", height=12)
        tree.heading("sno", text="S.No.")
        tree.heading("title", text="Title")
        tree.column("sno", width=60, anchor="center")
        tree.column("title", width=220, anchor="w")
        # populate
        for i, (t, _) in enumerate(self.notes, 1):
            tree.insert("", "end", values=(i, t))
        tree.pack(fill="none", expand=True)
        return right, tree

    def save_and_backup(self):
        write_diary(self.current_file, self.username,
                    self.password, self.notes)
        backup_file(self.current_file, self.current_code, make=True)

    def labeled_entry(self, parent, text, width=50):
        tk.Label(parent, text=text, bg=self.bg_color,
                 fg=self.fg_color).grid(sticky="w")
        e = tk.Entry(parent, width=width)
        e.grid(sticky="we", pady=(0, 8))
        return e

    def labeled_text(self, parent, text, width=60, height=12):
        tk.Label(parent, text=text, bg=self.bg_color,
                 fg=self.fg_color).grid(sticky="w")
        t = scrolledtext.ScrolledText(
            parent, wrap="word", width=width, height=height)
        t.grid(sticky="we", pady=(0, 8))
        return t

    # -------- main nav --------

    def show_main_menu(self):
        self.clear()
        tk.Label(
            self.page, text="Welcome to Diary", font=("Arial", 20),
            bg=self.bg_color, fg=self.fg_color
        ).pack(pady=10)
        tk.Button(self.page, text="Log In",
                  command=self.login).pack(fill="x", pady=3)
        tk.Button(self.page, text="Register",
                  command=self.register).pack(fill="x", pady=3)
        tk.Button(self.page, text="Change Colors",
                  command=self.change_colors).pack(fill="x", pady=3)
        tk.Button(self.page, text="Delete User",
                  command=self.delete_user).pack(fill="x", pady=3)
        tk.Button(self.page, text="Quit", command=self.quit).pack(
            fill="x", pady=3)

    def login(self):
        uid = simpledialog.askstring("Login", "Enter your ID (5 digits):")
        if not uid or not uid.isdigit() or len(uid) != 5:
            messagebox.showerror("Error", "ID must contain exactly 5 digits")
            return
        filename = f"{uid}.dat"
        if not os.path.exists(filename):
            messagebox.showerror("Error", "ID not found")
            return
        no, color, recs = get_all_user_records()
        enc_code = None
        for xuid, code in recs:
            if f"{xuid}.dat" == filename:
                enc_code = code
                break
        if not enc_code:
            messagebox.showerror("Error", "User code not found")
            return
        encrypt_file_in_place(filename, -1, enc_code)
        try:
            u, p, notes = read_diary(filename)
        except Exception as e:
            messagebox.showerror("Error", f"Corrupt diary: {e}")
            encrypt_file_in_place(filename, +1, enc_code)
            return
        username = simpledialog.askstring("Login", "Enter Username:")
        password = simpledialog.askstring("Login", "Enter Password:", show="*")
        if u != username or p != password:
            messagebox.showerror("Error", "Invalid credentials")
            encrypt_file_in_place(filename, +1, enc_code)
            return
        self.current_file = filename
        self.current_code = enc_code
        self.username = u
        self.password = p
        self.notes = notes
        backup_file(filename, enc_code, make=True)
        self.show_user_menu()

    def register(self):
        username = simpledialog.askstring(
            "Register", "Enter Username (max. 20):")
        if not username or len(username) > 20:
            messagebox.showerror(
                "Error", "Length of username should be in range (1-20)")
            return
        password = simpledialog.askstring(
            "Register", "Enter Password (max. 20):", show="*")
        if not password or len(password) > 20:
            messagebox.showerror(
                "Error", "Length of password should be in range (1-20)")
            return
        confirm = simpledialog.askstring(
            "Register", "Re-enter Password:", show="*")
        if password != confirm:
            messagebox.showerror("Error", "Passwords do not match")
            return
        no, color, recs = get_all_user_records()
        ids = {uid for uid, _ in recs}
        new_id = generate_unique_id(ids)
        new_code = generate_encryption_code()
        recs.append((new_id, new_code))
        write_all_user_records(no+1, color, recs)
        backup_file(SETTINGS_FILE, self.global_code, make=True)
        fname = f"{new_id}.dat"
        write_diary(fname, username, password, [])
        encrypt_file_in_place(fname, +1, new_code)
        messagebox.showinfo("Success", f"Your ID is {new_id}")

    def change_colors(self):
        """Dialog to change console colors (background & foreground)."""
        win = tk.Toplevel(self.root)
        win.title("Choose Console Colours")
        win.configure(bg=self.bg_color)

        tk.Label(win, text="Background:", bg=self.bg_color, fg=self.fg_color).grid(
            row=0, column=0, padx=10, pady=10, sticky="w")
        tk.Label(win, text="Foreground:", bg=self.bg_color, fg=self.fg_color).grid(
            row=1, column=0, padx=10, pady=10, sticky="w")

        # Background options (include custom G = #efefef)
        bg_options = [
            "G - #efefef (Default Light Gray)",
            "0 - Black",
            "1 - Blue",
            "2 - Green",
            "3 - Aqua",
            "4 - Red",
            "5 - Purple",
            "6 - Yellow",
            "7 - White",
            "8 - Gray",
            "9 - Light Blue",
        ]

        # Foreground options
        fg_options = [
            "0 - Black",
            "1 - Blue",
            "2 - Green",
            "3 - Aqua",
            "4 - Red",
            "5 - Purple",
            "6 - Yellow",
            "7 - White",
            "8 - Gray",
            "9 - Light Blue",
        ]

        # Figure out current selections
        bg_display = next(
            (opt for opt in bg_options if opt.startswith(self.bg_code)), bg_options[0])
        fg_display = next(
            (opt for opt in fg_options if opt.startswith(self.fg_code)), fg_options[0])

        bg_var = tk.StringVar(value=bg_display)
        fg_var = tk.StringVar(value=fg_display)

        bg_combo = ttk.Combobox(win, textvariable=bg_var,
                                values=bg_options, state="readonly", width=30)
        fg_combo = ttk.Combobox(win, textvariable=fg_var,
                                values=fg_options, state="readonly", width=30)

        bg_combo.grid(row=0, column=1, padx=10, pady=10)
        fg_combo.grid(row=1, column=1, padx=10, pady=10)

        def apply_colors():
            bg_code = bg_var.get().split(" ")[0]  # e.g. "G"
            fg_code = fg_var.get().split(" ")[0]  # e.g. "0"
            new_code = (bg_code + fg_code).encode()

            try:
                no_of_user, _, records = get_all_user_records()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to read settings: {e}")
                return

            # write back with new color code
            with open(SETTINGS_FILE, "wb") as f:
                f.write(struct.pack("<i", no_of_user))  # total users
                f.write(new_code)                       # new colors
                for uid, vals in records:               # restore user records
                    f.write(struct.pack("<i", uid))
                    f.write(struct.pack("<" + "h"*ENCRYPTION_ARRAY_SIZE, *vals))

            backup_file(SETTINGS_FILE, self.global_code)

            # reload theme
            self.bg_color, self.fg_color, self.bg_code, self.fg_code = self.decode_colors(
                new_code)
            self.root.configure(bg=self.bg_color)
            self.page.configure(bg=self.bg_color)

            messagebox.showinfo("Success", "Colors updated successfully!")
            win.destroy()
            self.show_main_menu()

        ttk.Button(win, text="Apply", command=apply_colors).grid(
            row=2, column=0, columnspan=2, pady=15)

    def apply_theme_to_widgets(self, parent=None):
        """Recursively apply bg/fg colors to all widgets inside parent (or root)."""
        if parent is None:
            parent = self.root

        for widget in parent.winfo_children():
            # Configure common widgets
            try:
                if isinstance(widget, (tk.Label, tk.Button, tk.Frame, tk.Toplevel)):
                    widget.configure(bg=self.bg_color, fg=self.fg_color)
                elif isinstance(widget, (tk.Entry, tk.Text)):
                    widget.configure(bg=self.bg_color, fg=self.fg_color,
                                     insertbackground=self.fg_color)  # cursor color
                elif isinstance(widget, ttk.Combobox):
                    style = ttk.Style()
                    style.configure("TCombobox",
                                    fieldbackground=self.bg_color,
                                    background=self.bg_color,
                                    foreground=self.fg_color)
            except Exception:
                pass  # some widgets may not support fg/bg

            # Recurse into children
            self.apply_theme_to_widgets(widget)

    def delete_user(self):
        uid = simpledialog.askstring("Delete User", "Enter ID:")
        if not uid or not uid.isdigit() or len(uid) != 5:
            messagebox.showerror("Error", "ID must contain exactly 5 digits")
            return
        filename = f"{uid}.dat"
        if not os.path.exists(filename):
            messagebox.showerror("Error", "ID not found")
            return
        no, color, recs = get_all_user_records()

        enc_code = None
        for xuid, code in recs:
            if f"{xuid}.dat" == filename:
                enc_code = code
                break
        if not enc_code:
            messagebox.showerror("Error", "User code not found")
            return
        encrypt_file_in_place(filename, -1, enc_code)
        try:
            u, p, notes = read_diary(filename)
        except Exception as e:
            messagebox.showerror("Error", f"Corrupt diary: {e}")
            encrypt_file_in_place(filename, +1, enc_code)
            return
        encrypt_file_in_place(filename, +1, enc_code)
        username = simpledialog.askstring("Login", "Enter Username:")
        password = simpledialog.askstring("Login", "Enter Password:", show="*")
        if u != username or p != password:
            messagebox.showerror("Error", "Invalid credentials")
            return

        target_id = int(uid)
        new_records = [(xid, code) for xid, code in recs if xid != target_id]
        write_all_user_records(max(0, no-1), color, new_records)
        backup_file(SETTINGS_FILE, self.global_code, make=True)
        try:
            os.remove(filename)
        except FileNotFoundError:
            pass
        messagebox.showinfo("Done", "User deleted")

    def show_user_menu(self):
        self.clear()
        header = tk.Frame(self.page, bg=self.bg_color)
        header.pack(fill="x", pady=(0, 10))
        tk.Label(header, text=f"Hello {self.username} :)", font=(
            "Arial", 18), bg=self.bg_color, fg=self.fg_color).pack(side="left")

        body = tk.Frame(self.page, bg=self.bg_color)
        body.pack(fill="none", expand=True)
        def btn(txt, cmd): return tk.Button(body, text=txt, command=cmd)
        btn("Write Note", self.write_note).pack(fill="x", pady=4)
        btn("Append Note", self.append_note).pack(fill="x", pady=4)
        btn("Read Note", self.read_note).pack(fill="x", pady=4)
        btn("Remove Note", self.remove_note).pack(fill="x", pady=4)
        btn("Change Username/Password",
            self.change_credentials).pack(fill="x", pady=4)
        btn("Log Out", self.logout).pack(fill="x", pady=4)

    # -------- Screens (match your mockups) --------
    def write_note(self):
        """Left: title + text; Right: table + 'slots available / where to insert'"""
        self.clear()
        container = tk.Frame(self.page, bg=self.bg_color)
        container.pack(fill="none", expand=True)

        # top bar
        tk.Button(container, text="Go Back", command=self.show_user_menu).grid(
            row=0, column=0, padx=5, pady=5, sticky="w")

        # left column
        left = tk.Frame(container, bg=self.bg_color)
        left.grid(row=1, column=0, sticky="nwe", padx=(5, 20), pady=5)
        left.grid_columnconfigure(0, weight=1)

        title_e = self.labeled_entry(left, "Enter Title")
        text_t = self.labeled_text(left, "Enter Text")

        # right column (table)
        right, tree = self.build_right_table(container)
        right.grid(row=1, column=1, sticky="n", padx=5, pady=5)

        # insert position controls
        total_slots = len(self.notes) + 1
        if len(self.notes) == 0:
            tk.Label(right, text="No content present\nEnter '1' as input here",
                     bg=self.bg_color, fg=self.fg_color).pack(pady=(8, 2), anchor="w")
        else:
            tk.Label(right, text=f"{total_slots} slots available\nWhere to insert from 1 to {total_slots}?", bg=self.bg_color, fg=self.fg_color).pack(
                pady=(8, 2), anchor="w")

        pos_e = tk.Entry(right, width=6)
        pos_e.pack(anchor="w", pady=(0, 10))

        def do_write():
            title = (title_e.get() or "").strip()[:50]
            text = (text_t.get("1.0", "end") or "").rstrip("\n")[:10000]
            if not title or not text:
                messagebox.showerror(
                    "Error", "Title and Text cannot be empty.")
                return
            try:
                pos = int(pos_e.get())
                if not (1 <= pos <= total_slots):
                    raise ValueError
            except ValueError:
                messagebox.showerror(
                    "Error", f"Position should be in range (1-{total_slots})")
                return
            self.notes.insert(pos-1, (title, text))
            self.save_and_backup()
            messagebox.showinfo("Done", f"Content Inserted in Slot {pos}...")
            self.write_note()  # refresh page

        tk.Button(left, text="Write", command=do_write).grid(
            sticky="w", pady=6)

    def append_note(self):
        if not self.notes:
            messagebox.showinfo("Info", "Nothing to append in...")
            return
        """Left: text box; Right: table + 'where to append'"""
        self.clear()
        container = tk.Frame(self.page, bg=self.bg_color)
        container.pack(fill="none", expand=True)
        tk.Button(container, text="Go Back", command=self.show_user_menu).grid(
            row=0, column=0, padx=5, pady=5, sticky="w")

        left = tk.Frame(container, bg=self.bg_color)
        left.grid(row=1, column=0, sticky="nwe", padx=(5, 20), pady=5)
        left.grid_columnconfigure(0, weight=1)

        text_t = self.labeled_text(left, "Enter Text")

        right, tree = self.build_right_table(container)
        right.grid(row=1, column=1, sticky="n", padx=5, pady=5)

        if len(self.notes) == 1:
            tk.Label(right, text="Only one content to append in\nEnter '1' as input here",
                     bg=self.bg_color, fg=self.fg_color).pack(pady=(8, 2), anchor="w")
        else:
            tk.Label(right, text=f"Where to append from 1 to {len(self.notes)}?", bg=self.bg_color, fg=self.fg_color).pack(
                pady=(8, 2), anchor="w")
        pos_e = tk.Entry(right, width=6)
        pos_e.pack(anchor="w", pady=(0, 10))

        def do_append():
            try:
                pos = int(pos_e.get())
                if not (1 <= pos <= len(self.notes)):
                    raise ValueError
            except ValueError:
                messagebox.showerror(
                    "Error", f"Position should be in range (1-{len(self.notes)})")
                return
            add = (text_t.get("1.0", "end") or "").rstrip("\n")[:10000]
            t, x = self.notes[pos-1]
            self.notes[pos-1] = (t, x + ("\n" if x and add else "") + add)
            self.save_and_backup()
            messagebox.showinfo("Done", f"Content Appended in Slot {pos}...")
            self.append_note()

        tk.Button(left, text="Append", command=do_append).grid(
            sticky="w", pady=6)

    def read_note(self):
        if not self.notes:
            messagebox.showinfo("Info", "Nothing to read...")
            return
        """Left: Title (readonly) + Text (readonly) + Read button; Right: table + 'what to read'"""
        self.clear()
        container = tk.Frame(self.page, bg=self.bg_color)
        container.pack(fill="none", expand=True)
        tk.Button(container, text="Go Back", command=self.show_user_menu).grid(
            row=0, column=0, padx=5, pady=5, sticky="w")

        left = tk.Frame(container, bg=self.bg_color)
        left.grid(row=1, column=0, sticky="nwe", padx=(5, 20), pady=5)
        left.grid_columnconfigure(0, weight=1)

        tk.Label(left, text="Title", bg=self.bg_color,
                 fg=self.fg_color).grid(sticky="w")
        title_e = tk.Entry(left, width=50, state="normal")
        title_e.grid(sticky="we", pady=(0, 8))

        tk.Label(left, text="Text", bg=self.bg_color,
                 fg=self.fg_color).grid(sticky="w")
        text_t = scrolledtext.ScrolledText(
            left, wrap="word", width=60, height=12)
        text_t.grid(sticky="we", pady=(0, 8))

        right, tree = self.build_right_table(container)
        right.grid(row=1, column=1, sticky="n", padx=5, pady=5)

        if len(self.notes) == 1:
            tk.Label(right, text="Only one content to read\nEnter '1' as input here",
                     bg=self.bg_color, fg=self.fg_color).pack(pady=(8, 2), anchor="w")
        else:
            tk.Label(right, text=f"What to read from 1 to {len(self.notes)}?", bg=self.bg_color, fg=self.fg_color).pack(
                pady=(8, 2), anchor="w")
        pos_e = tk.Entry(right, width=6)
        pos_e.pack(anchor="w", pady=(0, 10))

        def do_read():
            try:
                pos = int(pos_e.get())
                if not (1 <= pos <= len(self.notes)):
                    raise ValueError
            except ValueError:
                messagebox.showerror(
                    "Error", f"Position should be in range (1-{len(self.notes)})")
                return
            t, x = self.notes[pos-1]
            title_e.config(state="normal")
            title_e.delete(0, "end")
            title_e.insert(0, t)
            title_e.config(state="readonly")
            text_t.config(state="normal")
            text_t.delete("1.0", "end")
            text_t.insert("1.0", x)
            text_t.config(state="disabled")

        tk.Button(left, text="Read", command=do_read).grid(sticky="w", pady=6)

    def remove_note(self):
        if not self.notes:
            messagebox.showinfo("Info", "Nothing to delete...")
            return
        """Right: table + 'what to remove'; Left: just buttons like mockup"""
        self.clear()
        container = tk.Frame(self.page, bg=self.bg_color)
        container.pack(fill="none", expand=True)
        tk.Button(container, text="Go Back", command=self.show_user_menu).grid(
            row=0, column=0, padx=5, pady=5, sticky="w")

        left = tk.Frame(container, bg=self.bg_color)
        left.grid(row=1, column=0, sticky="nw", padx=(5, 30), pady=5)

        right, tree = self.build_right_table(container)
        right.grid(row=1, column=1, sticky="n", padx=5, pady=5)

        if len(self.notes) == 1:
            tk.Label(left, text="Only one content to remove\nEnter '1' as input here",
                     bg=self.bg_color, fg=self.fg_color).grid(sticky="w", pady=(0, 4))
        else:
            tk.Label(left, text=f"What to remove from 1 to {len(self.notes)}?", bg=self.bg_color, fg=self.fg_color).grid(
                sticky="w", pady=(0, 4))
        pos_e = tk.Entry(left, width=8)
        pos_e.grid(sticky="w")

        def do_remove():
            try:
                pos = int(pos_e.get())
                if not (1 <= pos <= len(self.notes)):
                    raise ValueError
            except ValueError:
                messagebox.showerror(
                    "Error", f"Position should be in range (1-{len(self.notes)})")
                return
            self.notes.pop(pos-1)
            self.save_and_backup()
            messagebox.showinfo("Done", f"Content in Slot {pos} Deleted...")

            if not self.notes:
                messagebox.showinfo("Info", "Now, there are no notes here")
                self.show_user_menu()
            else:
                self.remove_note()

        tk.Button(left, text="Remove", command=do_remove).grid(
            sticky="w", pady=10)

    def change_credentials(self):
        ch = simpledialog.askstring(
            "(u)sername or (p)assword?", "Type 'u' to change Username & Type 'p' to change Password")
        if ch == 'u':
            newu = simpledialog.askstring("Change", "New Username (max. 20):")
            if not newu or len(newu) > 20:
                messagebox.showerror(
                    "Error", "Length of username should be in range (1-20)")
                return
            else:
                self.username = newu
        elif ch == 'p':
            newp = simpledialog.askstring(
                "Change", "New Password (max. 20):", show="*")
            if not newp or len(newp) > 20:
                messagebox.showerror(
                    "Error", "Length of password should be in range (1-20)")
                return
            confirm_newp = simpledialog.askstring(
                "Change", "Re-enter Password:", show="*")
            if confirm_newp != newp:
                messagebox.showerror("Error", "Passwords do not match")
                return
            else:
                self.password = newp
        else:
            messagebox.showerror(
                "Error", "Input is neither 'u' not 'p'")
            return
        self.save_and_backup()
        messagebox.showinfo("Done", "Updated")

    def logout(self):
        encrypt_file_in_place(self.current_file, +1, self.current_code)
        backup_file(self.current_file, self.current_code, make=False)
        messagebox.showinfo("Logout", "Bye Bye !!!")
        self.current_file = None
        self.current_code = None
        self.username = None
        self.password = None
        self.notes = []
        self.show_main_menu()

    def quit(self):
        encrypt_settings(self.global_code)
        backup_file(SETTINGS_FILE, self.global_code, make=False)

        # Clear current page
        for widget in self.page.winfo_children():
            widget.destroy()

        # Show goodbye text on screen
        label = tk.Label(self.page, text="See you next time !!!",
                         font=("Arial", 36, "bold"),
                         bg=self.bg_color, fg=self.fg_color)
        label.pack(expand=True)

        # Delay a bit so user can see the message before exit
        self.root.after(2000, self.root.destroy)


if __name__ == "__main__":
    random.seed(int(time.time()))
    root = tk.Tk()
    root.geometry("900x520")  # comfortable default size for the layouts
    app = DiaryApp(root)
    root.mainloop()
