namespace The_Diary;

partial class Diary
{
    enum Operation { NOTHING, WRITE, APPEND, READ, REMOVE, USERPASS };
    private Operation mode;
    private Button Back_To_Dashboard = new();
    private Label Hello = new();

    private Button Write_a_Note = new();
    private Label new_title = new();
    private TextBox new_title_value = new();
    private Label new_text = new();
    private TextBox new_text_value = new();
    private Button Write_Button = new();
    private ListView write_where = new();
    private Label slot_pick = new();
    private TextBox slot_pick_value = new();

    private Button Append_in_Note = new();
    private Label append_text = new();
    private TextBox append_text_value = new();
    private Button Append_Button = new();
    private ListView append_where = new();
    private Label append_slot_pick = new();
    private TextBox append_slot_pick_value = new();

    private Button Read_a_Note = new();
    private Label read_title = new();
    private TextBox read_title_value = new();
    private Label read_text = new();
    private TextBox read_text_value = new();
    private Button Read_Button = new();
    private ListView read_what = new();
    private Label read_slot_pick = new();
    private TextBox read_slot_pick_value = new();

    private Button Remove_a_Note = new();
    private ListView remove_what = new();
    private Label remove_slot_pick = new();
    private TextBox remove_slot_pick_value = new();
    private Button Remove_Button = new();

    private Button Change_Username_Password = new();
    private GroupBox change_choices = new();
    private RadioButton change_username = new();
    private RadioButton change_password = new();
    private Label new_userpass = new();
    private TextBox new_userpass_value = new();
    private Button Change_Username_Password_Button = new();

    private Button Log_Out = new();

    private void User_Dashboard()
    {
        mode = Operation.NOTHING;
        Back_To_Dashboard = new Button
        {
            AutoSize = true,
            Text = "Go Back",
            Location = new Point(20, 10),
            Visible = false
        };
        Back_To_Dashboard.Click += new EventHandler(Back_To_Dashboard_Click);
        Hello = new Label
        {
            AutoSize = true,
            Font = new Font("Segoe UI", 20F, FontStyle.Regular, GraphicsUnit.Point),
            Location = new Point(20, 20),
            Visible = false
        };
        //
        //
        //
        Write_a_Note = new Button
        {
            Text = "Write a Note",
            Location = new Point(220, 100),
            Size = new Size(180, 30),
            Visible = false
        };
        Write_a_Note.Click += new EventHandler(Write_a_Note_Click);
        new_title = new Label
        {
            AutoSize = true,
            Text = "Enter Title",
            Location = new Point(20, 50),
            Visible = false
        };
        new_title_value = new TextBox
        {
            AutoSize = false,
            Location = new Point(120, 50),
            Size = new Size(500, 20),
            Visible = false
        };
        new_text = new Label
        {
            AutoSize = true,
            Text = "Enter Text",
            Location = new Point(20, 100),
            Visible = false
        };
        new_text_value = new TextBox
        {
            AutoSize = false,
            Location = new Point(120, 100),
            Size = new Size(500, 280),
            Multiline = true,
            WordWrap = true,
            ScrollBars = ScrollBars.Vertical,
            Visible = false
        };
        Write_Button = new Button
        {
            AutoSize = true,
            Text = "Write",
            Location = new Point(20, 380),
            Visible = false
        };
        Write_Button.Click += new EventHandler(Write_Button_Click);
        write_where = new ListView
        {
            Location = new Point(700, 50),
            Size = new Size(250, 200),
            View = View.Details,
            GridLines = true,
            Visible = false
        };
        slot_pick = new Label
        {
            AutoSize = true,
            Location = new Point(700, 300),
            Visible = false
        };
        slot_pick_value = new TextBox
        {
            Location = new Point(700, 350),
            Size = new Size(60, 20),
            Visible = false,
        };
        slot_pick_value.KeyPress += new KeyPressEventHandler(OnlyDigits);
        //
        //
        //
        Append_in_Note = new Button
        {
            Text = "Append in Note",
            Location = new Point(220, 150),
            Size = new Size(180, 30),
            Visible = false
        };
        Append_in_Note.Click += new EventHandler(Append_in_Note_Click);
        append_text = new Label
        {
            AutoSize = true,
            Text = "Enter Text",
            Location = new Point(20, 60),
            Visible = false
        };
        append_text_value = new TextBox
        {
            AutoSize = false,
            Location = new Point(20, 90),
            Size = new Size(500, 280),
            Multiline = true,
            WordWrap = true,
            ScrollBars = ScrollBars.Vertical,
            Visible = false
        };
        Append_Button = new Button
        {
            AutoSize = true,
            Text = "Append",
            Location = new Point(20, 380),
            Visible = false
        };
        Append_Button.Click += new EventHandler(Append_Button_Click);
        append_where = new ListView
        {
            Location = new Point(700, 50),
            Size = new Size(250, 200),
            View = View.Details,
            GridLines = true,
            Visible = false
        };
        append_slot_pick = new Label
        {
            AutoSize = true,
            Location = new Point(700, 300),
            Visible = false
        };
        append_slot_pick_value = new TextBox
        {
            Location = new Point(700, 350),
            Size = new Size(60, 20),
            Visible = false,
        };
        append_slot_pick_value.KeyPress += new KeyPressEventHandler(OnlyDigits);
        //
        //
        //
        Read_a_Note = new Button
        {
            Text = "Read a Note",
            Location = new Point(220, 200),
            Size = new Size(180, 30),
            Visible = false
        };
        Read_a_Note.Click += new EventHandler(Read_a_Note_Click);
        read_title = new Label
        {
            AutoSize = true,
            Text = "Title",
            Location = new Point(20, 50),
            Visible = false
        };
        read_title_value = new TextBox
        {
            AutoSize = false,
            Location = new Point(120, 50),
            Size = new Size(500, 20),
            ReadOnly = true,
            Visible = false
        };
        read_text = new Label
        {
            AutoSize = true,
            Text = "Text",
            Location = new Point(20, 100),
            Visible = false
        };
        read_text_value = new TextBox
        {
            AutoSize = false,
            Location = new Point(120, 100),
            Size = new Size(500, 280),
            Multiline = true,
            WordWrap = true,
            ScrollBars = ScrollBars.Vertical,
            ReadOnly = true,
            Visible = false
        };
        Read_Button = new Button
        {
            AutoSize = true,
            Text = "Read",
            Location = new Point(20, 380),
            Visible = false
        };
        Read_Button.Click += new EventHandler(Read_Button_Click);
        read_what = new ListView
        {
            Location = new Point(700, 50),
            Size = new Size(250, 200),
            View = View.Details,
            GridLines = true,
            Visible = false
        };
        read_slot_pick = new Label
        {
            AutoSize = true,
            Location = new Point(700, 300),
            Visible = false
        };
        read_slot_pick_value = new TextBox
        {
            Location = new Point(700, 350),
            Size = new Size(60, 20),
            Visible = false,
        };
        read_slot_pick_value.KeyPress += new KeyPressEventHandler(OnlyDigits);
        //
        //
        //
        Remove_a_Note = new Button
        {
            Text = "Remove Note",
            Location = new Point(220, 250),
            Size = new Size(180, 30),
            Visible = false
        };
        Remove_a_Note.Click += new EventHandler(Remove_a_Note_Click);
        remove_what = new ListView
        {
            Location = new Point(20, 80),
            Size = new Size(250, 200),
            View = View.Details,
            GridLines = true,
            Visible = false
        };
        remove_slot_pick = new Label
        {
            AutoSize = true,
            Location = new Point(20, 330),
            Visible = false
        };
        remove_slot_pick_value = new TextBox
        {
            Location = new Point(20, 380),
            Size = new Size(60, 20),
            Visible = false,
        };
        remove_slot_pick_value.KeyPress += new KeyPressEventHandler(OnlyDigits);
        Remove_Button = new Button
        {
            AutoSize = true,
            Text = "Remove",
            Location = new Point(150, 380),
            Visible = false
        };
        Remove_Button.Click += new EventHandler(Remove_Button_Click);
        //
        //
        //
        Change_Username_Password = new Button
        {
            Text = "Change Username/Password",
            Location = new Point(220, 300),
            Size = new Size(180, 30),
            Visible = false
        };
        Change_Username_Password.Click += new EventHandler(Change_Username_Password_Click);
        change_choices = new GroupBox
        {
            Location = new Point(50, 50),
            Size = new Size(200, 150),
            Text = "Choose From Below",
            Visible = false
        };
        change_username = new RadioButton
        {
            AutoSize = true,
            Location = new Point(40, 50),
            Text = "Username",
            Visible = false
        };
        change_username.CheckedChanged += new EventHandler((object? sender, EventArgs e) =>
        {
            if (change_username.Checked == true)
                new_userpass.Text = "Enter New Username";

        });
        change_password = new RadioButton
        {
            AutoSize = true,
            Location = new Point(40, 90),
            Text = "Password",
            Visible = false
        };
        change_password.CheckedChanged += new EventHandler((object? sender, EventArgs e) =>
        {
            if (change_password.Checked == true)
                new_userpass.Text = "Enter New Password";

        });
        change_choices.Controls.Add(change_username);
        change_choices.Controls.Add(change_password);
        new_userpass = new Label
        {
            AutoSize = true,
            Location = new Point(50, 220),
            Visible = false
        };
        new_userpass_value = new TextBox
        {
            Location = new Point(50, 250),
            Size = new Size(140, 25),
            MaxLength = 20,
            Visible = false
        };
        Change_Username_Password_Button = new Button
        {
            AutoSize = true,
            Location = new Point(50, 300),
            Text = "Change",
            Visible = false
        };
        Change_Username_Password_Button.Click += new EventHandler(Change_Username_Password_Button_Click);
        //
        //
        //
        Log_Out = new Button
        {
            Text = "Log Out",
            Location = new Point(220, 350),
            Size = new Size(180, 30),
            Visible = false
        };
        Log_Out.Click += new EventHandler(Log_Out_Click);

        SuspendLayout();
        Controls.Add(Back_To_Dashboard);
        Controls.Add(Hello);

        Controls.Add(Write_a_Note);
        Controls.Add(new_title);
        Controls.Add(new_title_value);
        Controls.Add(new_text);
        Controls.Add(new_text_value);
        Controls.Add(Write_Button);
        Controls.Add(write_where);
        Controls.Add(slot_pick);
        Controls.Add(slot_pick_value);

        Controls.Add(Append_in_Note);
        Controls.Add(append_text);
        Controls.Add(append_text_value);
        Controls.Add(Append_Button);
        Controls.Add(append_where);
        Controls.Add(append_slot_pick);
        Controls.Add(append_slot_pick_value);

        Controls.Add(Read_a_Note);
        Controls.Add(read_title);
        Controls.Add(read_title_value);
        Controls.Add(read_text);
        Controls.Add(read_text_value);
        Controls.Add(Read_Button);
        Controls.Add(read_what);
        Controls.Add(read_slot_pick);
        Controls.Add(read_slot_pick_value);

        Controls.Add(Remove_a_Note);
        Controls.Add(remove_what);
        Controls.Add(remove_slot_pick);
        Controls.Add(remove_slot_pick_value);
        Controls.Add(Remove_Button);

        Controls.Add(Change_Username_Password);
        Controls.Add(change_choices);
        Controls.Add(new_userpass);
        Controls.Add(new_userpass_value);
        Controls.Add(Change_Username_Password_Button);

        Controls.Add(Log_Out);

        ResumeLayout(false);
    }
    private void Back_To_Dashboard_Click(object? sender, EventArgs e)
    {
        if (mode == Operation.WRITE)
        {
            new_title.Visible = false;
            new_title_value.Text = "";
            new_title_value.Visible = false;
            new_text.Visible = false;
            new_text_value.Text = "";
            new_text_value.Visible = false;
            Write_Button.Visible = false;
            write_where.Items.Clear();
            write_where.Columns.Clear();
            write_where.Visible = false;
            slot_pick.Text = "";
            slot_pick.Visible = false;
            slot_pick_value.Text = "";
            slot_pick_value.Visible = false;

            Size = new Size(640, 480);
        }
        else if (mode == Operation.APPEND)
        {
            append_text.Visible = false;
            append_text_value.Text = "";
            append_text_value.Visible = false;
            Append_Button.Visible = false;
            append_where.Items.Clear();
            append_where.Columns.Clear();
            append_where.Visible = false;
            append_slot_pick.Text = "";
            append_slot_pick.Visible = false;
            append_slot_pick_value.Text = "";
            append_slot_pick_value.Visible = false;

            Size = new Size(640, 480);
        }
        else if (mode == Operation.READ)
        {
            read_title.Visible = false;
            read_title_value.Text = "";
            read_title_value.Visible = false;
            read_text.Visible = false;
            read_text_value.Text = "";
            read_text_value.Visible = false;
            Read_Button.Visible = false;
            read_what.Items.Clear();
            read_what.Columns.Clear();
            read_what.Visible = false;
            read_slot_pick.Text = "";
            read_slot_pick.Visible = false;
            read_slot_pick_value.Text = "";
            read_slot_pick_value.Visible = false;

            Size = new Size(640, 480);
        }
        else if (mode == Operation.REMOVE)
        {
            remove_what.Items.Clear();
            remove_what.Columns.Clear();
            remove_what.Visible = false;
            remove_slot_pick.Text = "";
            remove_slot_pick.Visible = false;
            remove_slot_pick_value.Text = "";
            remove_slot_pick_value.Visible = false;
            Remove_Button.Visible = false;
        }
        else if (mode == Operation.USERPASS)
        {
            change_choices.Visible = false;
            change_username.Visible = false;
            change_password.Visible = false;
            new_userpass.Text = "";
            new_userpass.Visible = false;
            new_userpass_value.Text = "";
            new_userpass_value.Visible = false;
            Change_Username_Password_Button.Visible = false;
        }
        mode = Operation.NOTHING;
        Back_To_Dashboard.Visible = false;

        Hello.Visible = true;
        Write_a_Note.Visible = true;
        Append_in_Note.Visible = true;
        Read_a_Note.Visible = true;
        Remove_a_Note.Visible = true;
        Change_Username_Password.Visible = true;
        Log_Out.Visible = true;
    }
    private void OnlyDigits(object? sender, KeyPressEventArgs e)
    {
        // 8 is ASCII value of Backspace key
        if (!char.IsDigit(e.KeyChar) && e.KeyChar != Convert.ToChar(8))
            e.Handled = true;
        else
            e.Handled = false;
    }
    private void Display(ListView content)
    {
        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        binReader.ReadString();
        binReader.ReadString();
        binReader.ReadInt32();
        int TitleWidth = 0;
        for (int i = 0; i < amount_of_content; i++)
        {
            ListViewItem S_No = new(Convert.ToString(i + 1));
            content.Items.Add(S_No);
            string Title = binReader.ReadString();
            if (Title.Length > TitleWidth)
                TitleWidth = Title.Length;
            S_No.SubItems.Add(Title);
            binReader.ReadString();
        }
        binReader.Close();
        content.Columns.Add("S.No.", 40 + Convert.ToString(amount_of_content).Length * 5);
        content.Columns.Add("Title", 40 + TitleWidth * 11);
        content.Visible = true;
    }

    private void Write_a_Note_Click(object? sender, EventArgs e)
    {
        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        mode = Operation.WRITE;
        Back_To_Dashboard.Visible = true;
        new_title.Visible = true;
        new_title_value.Visible = true;
        new_text.Visible = true;
        new_text_value.Visible = true;
        Write_Button.Visible = true;
        Display(write_where);

        if (amount_of_content == 0)
            slot_pick.Text = "No content present" + "\n" + "Enter '1' as input here";
        else
            slot_pick.Text = "" + (amount_of_content + 1) + " slots available" + "\n" + "Where to insert from 1 to " + (amount_of_content + 1) + "?";
        slot_pick.Visible = true;
        slot_pick_value.Visible = true;

        Size = new Size(1000, 480);
    }
    private void Write_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(new_title_value.Text) || string.IsNullOrEmpty(new_text_value.Text) || string.IsNullOrEmpty(slot_pick_value.Text))
        {
            MessageBox.Show("Everything must be filled", "Notice");
            return;
        }
        if (!(Convert.ToInt32(slot_pick_value.Text) >= 1 && Convert.ToInt32(slot_pick_value.Text) <= amount_of_content + 1))
        {
            MessageBox.Show("Slot Pick should be within the range", "Notice");
            slot_pick_value.Text = "";
            return;
        }
        int position = Convert.ToInt32(slot_pick_value.Text);
        amount_of_content++;

        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        BinaryWriter binWriter = new(File.Open("temp.dat", FileMode.Create));
        binWriter.Write(binReader.ReadString());
        binWriter.Write(binReader.ReadString());
        binReader.ReadInt32();
        binWriter.Write(amount_of_content);
        for (int i = 1; i < position; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Write(new_title_value.Text);
        binWriter.Write(new_text_value.Text);
        for (int i = position; i < amount_of_content; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Close();
        binReader.Close();

        File.Delete(Convert.ToString(ID) + ".dat");
        File.Move("temp.dat", Convert.ToString(ID) + ".dat");

        Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);
        BackUp(Convert.ToString(ID) + ".dat", Decision.MAKE_THE_BACKUP_FILE);
        Encryption(Convert.ToString(ID) + ".dat", Decision.DECRYPT, encryptionCode);
        MessageBox.Show("Content Written!!!", "Message");

        new_title_value.Text = "";
        new_text_value.Text = "";
        write_where.Items.Clear();
        write_where.Columns.Clear();
        Display(write_where);
        slot_pick.Text = "" + (amount_of_content + 1) + " slots available" + "\n" + "Where to insert from 1 to " + (amount_of_content + 1) + "?";
        slot_pick_value.Text = "";
    }

    private void Append_in_Note_Click(object? sender, EventArgs e)
    {
        if (amount_of_content == 0)
        {
            MessageBox.Show("Nothing to append in", "Notice");
            return;
        }
        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        mode = Operation.APPEND;
        Back_To_Dashboard.Visible = true;
        append_text.Visible = true;
        append_text_value.Visible = true;
        Append_Button.Visible = true;
        Display(append_where);

        if (amount_of_content == 1)
            append_slot_pick.Text = "Only one content to append in" + "\n" + "Enter '1' as input here";
        else
            append_slot_pick.Text = "Where to append from 1 to " + amount_of_content + "?";
        append_slot_pick.Visible = true;
        append_slot_pick_value.Visible = true;

        Size = new Size(1000, 480);
    }
    private void Append_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(append_text_value.Text) || string.IsNullOrEmpty(append_slot_pick_value.Text))
        {
            MessageBox.Show("Everything must be filled", "Notice");
            return;
        }
        if (!(Convert.ToInt32(append_slot_pick_value.Text) >= 1 && Convert.ToInt32(append_slot_pick_value.Text) <= amount_of_content))
        {
            MessageBox.Show("Slot Pick should be within the range", "Notice");
            append_slot_pick_value.Text = "";
            return;
        }
        int position = Convert.ToInt32(append_slot_pick_value.Text);

        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        BinaryWriter binWriter = new(File.Open("temp.dat", FileMode.Create));
        binWriter.Write(binReader.ReadString());
        binWriter.Write(binReader.ReadString());
        binReader.ReadInt32();
        binWriter.Write(amount_of_content);
        for (int i = 1; i < position; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Write(binReader.ReadString());
        binWriter.Write(binReader.ReadString() + append_text_value.Text);
        for (int i = position; i < amount_of_content; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Close();
        binReader.Close();

        File.Delete(Convert.ToString(ID) + ".dat");
        File.Move("temp.dat", Convert.ToString(ID) + ".dat");

        Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);
        BackUp(Convert.ToString(ID) + ".dat", Decision.MAKE_THE_BACKUP_FILE);
        Encryption(Convert.ToString(ID) + ".dat", Decision.DECRYPT, encryptionCode);
        MessageBox.Show("Text Appended!!!", "Message");

        append_text_value.Text = "";
        append_slot_pick_value.Text = "";
    }

    private void Read_a_Note_Click(object? sender, EventArgs e)
    {
        if (amount_of_content == 0)
        {
            MessageBox.Show("Nothing to read", "Notice");
            return;
        }
        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        mode = Operation.READ;
        Back_To_Dashboard.Visible = true;
        read_title.Visible = true;
        read_title_value.Visible = true;
        read_text.Visible = true;
        read_text_value.Visible = true;
        Read_Button.Visible = true;
        Display(read_what);

        if (amount_of_content == 1)
            read_slot_pick.Text = "Only one content to read" + "\n" + "Enter '1' as input here";
        else
            read_slot_pick.Text = "What to read from 1 to " + amount_of_content + "?";
        read_slot_pick.Visible = true;
        read_slot_pick_value.Visible = true;

        Size = new Size(1000, 480);
    }
    private void Read_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(read_slot_pick_value.Text))
        {
            MessageBox.Show("Choose what to read", "Notice");
            return;
        }
        if (!(Convert.ToInt32(read_slot_pick_value.Text) >= 1 && Convert.ToInt32(read_slot_pick_value.Text) <= amount_of_content))
        {
            MessageBox.Show("Slot Pick should be within the range", "Notice");
            read_slot_pick_value.Text = "";
            return;
        }
        int position = Convert.ToInt32(read_slot_pick_value.Text);

        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        binReader.ReadString();
        binReader.ReadString();
        binReader.ReadInt32();
        for (int i = 1; i < position; i++)
        {
            binReader.ReadString();
            binReader.ReadString();
        }
        read_title_value.Text = binReader.ReadString();
        read_text_value.Text = binReader.ReadString();
        binReader.Close();

        MessageBox.Show("Content Retrieved!!!", "Message");
        read_slot_pick_value.Text = "";
    }

    private void Remove_a_Note_Click(object? sender, EventArgs e)
    {
        if (amount_of_content == 0)
        {
            MessageBox.Show("Nothing to delete", "Notice");
            return;
        }
        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        mode = Operation.REMOVE;
        Back_To_Dashboard.Visible = true;
        Display(remove_what);

        if (amount_of_content == 1)
            remove_slot_pick.Text = "Only one content to remove" + "\n" + "Enter '1' as input here";
        else
            remove_slot_pick.Text = "What to remove from 1 to " + amount_of_content + "?";
        remove_slot_pick.Visible = true;
        remove_slot_pick_value.Visible = true;

        Remove_Button.Visible = true;
    }
    private void Remove_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(remove_slot_pick_value.Text))
        {
            MessageBox.Show("Choose what to remove", "Notice");
            return;
        }
        if (!(Convert.ToInt32(remove_slot_pick_value.Text) >= 1 && Convert.ToInt32(remove_slot_pick_value.Text) <= amount_of_content))
        {
            MessageBox.Show("Slot Pick should be within the range", "Notice");
            remove_slot_pick_value.Text = "";
            return;
        }
        int position = Convert.ToInt32(remove_slot_pick_value.Text);
        amount_of_content--;

        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        BinaryWriter binWriter = new(File.Open("temp.dat", FileMode.Create));
        binWriter.Write(binReader.ReadString());
        binWriter.Write(binReader.ReadString());
        binReader.ReadInt32();
        binWriter.Write(amount_of_content);
        for (int i = 1; i < position; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binReader.ReadString();
        binReader.ReadString();
        for (int i = position; i <= amount_of_content; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Close();
        binReader.Close();

        File.Delete(Convert.ToString(ID) + ".dat");
        File.Move("temp.dat", Convert.ToString(ID) + ".dat");

        Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);
        BackUp(Convert.ToString(ID) + ".dat", Decision.MAKE_THE_BACKUP_FILE);
        Encryption(Convert.ToString(ID) + ".dat", Decision.DECRYPT, encryptionCode);
        MessageBox.Show("Content Deleted!!!", "Message");

        remove_what.Items.Clear();
        remove_what.Columns.Clear();
        Display(remove_what);
        if (amount_of_content == 0)
            Back_To_Dashboard.PerformClick();
        else if (amount_of_content == 1)
            remove_slot_pick.Text = "Only one content to remove" + "\n" + "Enter '1' as input here";
        else
            remove_slot_pick.Text = "What to remove from 1 to " + amount_of_content + "?";
        remove_slot_pick_value.Text = "";
    }

    private void Change_Username_Password_Click(object? sender, EventArgs e)
    {
        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        mode = Operation.USERPASS;
        Back_To_Dashboard.Visible = true;
        change_choices.Visible = true;
        change_username.Checked = true;
        change_password.Checked = false;
        change_username.Visible = true;
        change_password.Visible = true;
        new_userpass.Text = "Enter New Username";
        new_userpass.Visible = true;
        new_userpass_value.Visible = true;
        Change_Username_Password_Button.Visible = true;
    }
    private void Change_Username_Password_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(new_userpass_value.Text))
        {
            MessageBox.Show("You must fill something", "Notice");
            return;
        }
        BinaryReader binReader = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Open));
        BinaryWriter binWriter = new(File.Open("temp.dat", FileMode.Create));
        if (change_username.Checked == true)
        {
            binWriter.Write(new_userpass_value.Text);
            binReader.ReadString();
            binWriter.Write(binReader.ReadString());
        }
        else if (change_password.Checked == true)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(new_userpass_value.Text);
            binReader.ReadString();
        }
        binWriter.Write(binReader.ReadInt32());
        for (int i = 0; i < amount_of_content; i++)
        {
            binWriter.Write(binReader.ReadString());
            binWriter.Write(binReader.ReadString());
        }
        binWriter.Close();
        binReader.Close();

        File.Delete(Convert.ToString(ID) + ".dat");
        File.Move("temp.dat", Convert.ToString(ID) + ".dat");

        Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);
        BackUp(Convert.ToString(ID) + ".dat", Decision.MAKE_THE_BACKUP_FILE);
        Encryption(Convert.ToString(ID) + ".dat", Decision.DECRYPT, encryptionCode);

        if (change_username.Checked == true)
            MessageBox.Show("Username changed", "Message");
        else if (change_password.Checked == true)
            MessageBox.Show("Password changed", "Message");
    }

    private void Log_Out_Click(object? sender, EventArgs e)
    {
        BackUp(Convert.ToString(ID) + ".dat", Decision.REMOVE_THE_BACKUP_FILE);
        Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);

        ID = 0;
        amount_of_content = 0;
        for (int i = 0; i < 256; i++)
            encryptionCode[i] = 0;
        MessageBox.Show("Bye Bye !!!", "Message");

        Hello.Visible = false;
        Write_a_Note.Visible = false;
        Append_in_Note.Visible = false;
        Read_a_Note.Visible = false;
        Remove_a_Note.Visible = false;
        Change_Username_Password.Visible = false;
        Log_Out.Visible = false;

        Welcome.Visible = true;
        Log_In.Visible = true;
        Register_a_User.Visible = true;
        Change_Form_Colour.Visible = true;
        Delete_a_User.Visible = true;
        Quit.Visible = true;
    }
}