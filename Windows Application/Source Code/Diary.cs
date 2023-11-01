using Microsoft.VisualBasic;

namespace The_Diary;

public partial class Diary : Form
{
    int no_of_users;
    byte color_background, color_foreground;
    byte[] encryptSettings = new byte[256];
    int ID, amount_of_content;
    byte[] encryptionCode = new byte[256];
    private void Generate_encryptionCode()
    {
        Random rand = new();
        int i, j, random;
        byte[] oneTo256 = new byte[256];
        for (i = 0; i < 256; i++)
            oneTo256[i] = (byte)i;
        for (i = 0; i < 256; i++)
        {
            random = rand.Next(0, 256 - i);
            encryptionCode[i] = oneTo256[random];
            for (j = (byte)random + 1; j < 256 - i; j++)
                oneTo256[j - 1] = oneTo256[j];
        }
    }
    private void SetFormColor()
    {
        if (color_background == 0)
            BackColor = SystemColors.Control;
        else if (color_background == 1)
            BackColor = Color.FromKnownColor(KnownColor.Black);
        else if (color_background == 2)
            BackColor = Color.FromKnownColor(KnownColor.White);
        else if (color_background == 3)
            BackColor = Color.FromKnownColor(KnownColor.Red);
        else if (color_background == 4)
            BackColor = Color.FromKnownColor(KnownColor.LightGreen);
        else if (color_background == 5)
            BackColor = Color.FromKnownColor(KnownColor.LightBlue);
        else if (color_background == 6)
            BackColor = Color.FromKnownColor(KnownColor.Yellow);
        if (color_foreground == 0)
            ForeColor = SystemColors.Control;
        else if (color_foreground == 1)
            ForeColor = Color.FromKnownColor(KnownColor.Black);
        else if (color_foreground == 2)
            ForeColor = Color.FromKnownColor(KnownColor.White);
        else if (color_foreground == 3)
            ForeColor = Color.FromKnownColor(KnownColor.Red);
        else if (color_foreground == 4)
            ForeColor = Color.FromKnownColor(KnownColor.LightGreen);
        else if (color_foreground == 5)
            ForeColor = Color.FromKnownColor(KnownColor.LightBlue);
        else if (color_foreground == 6)
            ForeColor = Color.FromKnownColor(KnownColor.Yellow);

        foreach (var button in Controls.OfType<Button>())
        {
            button.BackColor = SystemColors.Control;
            button.UseVisualStyleBackColor = true;
            button.ForeColor = Color.FromKnownColor(KnownColor.Black);
        }
    }
    enum Decision { ENCRYPT, DECRYPT, MAKE_THE_BACKUP_FILE, REMOVE_THE_BACKUP_FILE };
    private static void Encryption(string fileName, Decision direction, byte[] encrypt)
    {
        byte[] Content = File.ReadAllBytes(fileName);
        for (int i = 0; i < Content.Length; i++)
        {
            int val = Content[i];
            if (direction == Decision.ENCRYPT)
            {
                val += encrypt[i % 256];
                if (val > 255)
                    val -= 256;
            }
            else if (direction == Decision.DECRYPT)
            {
                val -= encrypt[i % 256];
                if (val < 0)
                    val += 256;
            }
            Content[i] = (byte)val;
        }
        File.WriteAllBytes(fileName, Content);
    }
    private static void BackUp(string fileName, Decision choice)
    {
        if (choice == Decision.MAKE_THE_BACKUP_FILE)
        {
            if (File.Exists("Backup/" + fileName))
            {
                byte[] data = File.ReadAllBytes(fileName);
                File.WriteAllBytes("Backup/" + fileName, data);
            }
            else
                File.Copy(fileName, "Backup/" + fileName);
        }
        else if (choice == Decision.REMOVE_THE_BACKUP_FILE)
            File.Delete("Backup/" + fileName);
    }
    public Diary()
    {
        Directory.CreateDirectory("Backup");
        BinaryReader Get_encryptSettings = new(File.Open("Encryption Code.dat", FileMode.Open));
        for (int i = 0; i < 256; i++)
            encryptSettings[i] = Get_encryptSettings.ReadByte();
        Get_encryptSettings.Close();

        InitializeComponent();
        User_Dashboard();

        if (File.Exists("settings.dat"))
        {
            BackUp("settings.dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption("settings.dat", Decision.DECRYPT, encryptSettings);

            BinaryReader binReader = new(File.Open("settings.dat", FileMode.Open));
            no_of_users = binReader.ReadInt32();
            color_background = binReader.ReadByte();
            color_foreground = binReader.ReadByte();
            binReader.Close();
            SetFormColor();
        }
        else
        {
            BinaryWriter binWriter = new(File.Open("settings.dat", FileMode.Create));
            no_of_users = 0;
            binWriter.Write(no_of_users);
            color_background = 0;
            binWriter.Write(color_background);
            color_foreground = 1;
            binWriter.Write(color_foreground);
            binWriter.Close();

            Encryption("settings.dat", Decision.ENCRYPT, encryptSettings);
            BackUp("settings.dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption("settings.dat", Decision.DECRYPT, encryptSettings);
        }
    }
    private void Log_In_Click(object? sender, EventArgs e)
    {
        if (no_of_users == 0)
            MessageBox.Show("No user available", "Notice");
        else
        {
            Welcome.Visible = false;
            Log_In.Visible = false;
            Register_a_User.Visible = false;
            Change_Form_Colour.Visible = false;
            Delete_a_User.Visible = false;
            Quit.Visible = false;

            Back_To_Main_Menu.Visible = true;
            login_id.Visible = true;
            login_id_value.Text = "";
            login_id_value.Visible = true;
            login_username.Visible = true;
            login_username_value.Text = "";
            login_username_value.Visible = true;
            login_password.Visible = true;
            login_password_value.Text = "";
            login_password_value.Visible = true;
            Login_Button.Visible = true;
        }
    }
    private void Login_Button_Click(object sender, EventArgs e)
    {
        if (!File.Exists(login_id_value.Text + ".dat"))
        {
            MessageBox.Show("ID doesn't exist", "Error");
            return;
        }
        BinaryReader binReader = new(File.Open("settings.dat", FileMode.Open));
        binReader.ReadInt32();
        binReader.ReadByte();
        binReader.ReadByte();
        for (int i = 0; i < no_of_users; i++)
        {
            int desiredID = binReader.ReadInt32();
            if (desiredID == Convert.ToInt32(login_id_value.Text))
            {
                for (int j = 0; j < 256; j++)
                    encryptionCode[j] = binReader.ReadByte();
                break;
            }
            else
            {
                for (int j = 0; j < 256; j++)
                    binReader.ReadByte();
            }
        }
        binReader.Close();

        Encryption(login_id_value.Text + ".dat", Decision.DECRYPT, encryptionCode);
        binReader = new(File.Open(login_id_value.Text + ".dat", FileMode.Open));
        string user = binReader.ReadString();
        string pass = binReader.ReadString();
        int getAmount = binReader.ReadInt32();
        binReader.Close();
        Encryption(login_id_value.Text + ".dat", Decision.ENCRYPT, encryptionCode);
        if (!user.Equals(login_username_value.Text) || !pass.Equals(login_password_value.Text))
        {
            MessageBox.Show("Both username & password must be correct", "Error");
            return;
        }
        else
        {
            BackUp(login_id_value.Text + ".dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption(login_id_value.Text + ".dat", Decision.DECRYPT, encryptionCode);

            ID = Convert.ToInt32(login_id_value.Text);
            amount_of_content = getAmount;
            MessageBox.Show("Successfully Logged In !!!", "Message");

            login_id_value.Text = "";
            login_username_value.Text = "";
            login_password_value.Text = "";
            Back_To_Main_Menu.Visible = false;
            login_id.Visible = false;
            login_id_value.Visible = false;
            login_username.Visible = false;
            login_username_value.Visible = false;
            login_password.Visible = false;
            login_password_value.Visible = false;
            Login_Button.Visible = false;

            Hello.Text = "Hello " + user + " :)";
            Hello.Visible = true;
            Write_a_Note.Visible = true;
            Append_in_Note.Visible = true;
            Read_a_Note.Visible = true;
            Remove_a_Note.Visible = true;
            Change_Username_Password.Visible = true;
            Log_Out.Visible = true;
        }
    }

    private void Register_a_User_Click(object? sender, EventArgs e)
    {
        Welcome.Visible = false;
        Log_In.Visible = false;
        Register_a_User.Visible = false;
        Change_Form_Colour.Visible = false;
        Delete_a_User.Visible = false;
        Quit.Visible = false;

        Back_To_Main_Menu.Visible = true;
        new_username.Visible = true;
        new_username_value.Text = "";
        new_username_value.Visible = true;
        new_password.Visible = true;
        new_password_value.Text = "";
        new_password_value.Visible = true;
        reEnter_new_password.Visible = true;
        reEnter_new_password_value.Text = "";
        reEnter_new_password_value.Visible = true;
        Register_Button.Visible = true;
    }
    private void Register_Button_Click(object? sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(new_username_value.Text) || string.IsNullOrEmpty(new_password_value.Text))
            MessageBox.Show("Both username & password must be filled", "Error");
        else if (!reEnter_new_password_value.Text.Equals(new_password_value.Text))
            MessageBox.Show("Passwords don't match", "Error");
        else
        {
            Random rand = new();
            do
            {
                ID = rand.Next(10000, 100000);
            }
            while (File.Exists(Convert.ToString(ID) + ".dat"));
            int getAmount = 0;
            Generate_encryptionCode();

            BinaryWriter binWriter = new(File.Open(Convert.ToString(ID) + ".dat", FileMode.Create));
            binWriter.Write(new_username_value.Text);
            binWriter.Write(new_password_value.Text);
            binWriter.Write(getAmount);
            binWriter.Close();
            Encryption(Convert.ToString(ID) + ".dat", Decision.ENCRYPT, encryptionCode);

            no_of_users++;
            binWriter = new(File.Open("settings.dat", FileMode.Open));
            binWriter.Write(no_of_users);
            binWriter.Seek(2 * sizeof(byte), SeekOrigin.Current);
            for (int i = 1; i < no_of_users; i++)
                binWriter.Seek(sizeof(int) + (256 * sizeof(byte)), SeekOrigin.Current);
            binWriter.Write(ID);
            for (int i = 0; i < 256; i++)
                binWriter.Write(encryptionCode[i]);
            binWriter.Close();

            Encryption("settings.dat", Decision.ENCRYPT, encryptSettings);
            BackUp("settings.dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption("settings.dat", Decision.DECRYPT, encryptSettings);

            MessageBox.Show("You Are Registered !!!", "Message");
            Interaction.InputBox("Your ID is " + ID, "Unique Identification Number", Convert.ToString(ID));

            ID = 0;
            for (int i = 0; i < 256; i++)
                encryptionCode[i] = 0;
            new_username_value.Text = "";
            new_password_value.Text = "";
            reEnter_new_password_value.Text = "";
        }
    }

    private void Change_Form_Colour_Click(object? sender, EventArgs e)
    {
        Welcome.Visible = false;
        Log_In.Visible = false;
        Register_a_User.Visible = false;
        Change_Form_Colour.Visible = false;
        Delete_a_User.Visible = false;
        Quit.Visible = false;

        Back_To_Main_Menu.Visible = true;
        background_color.Visible = true;
        background_color_value.Text = background_color_value.Items[color_background].ToString();
        background_color_value.Visible = true;
        foreground_color_value.Text = foreground_color_value.Items[color_foreground].ToString();
        foreground_color.Visible = true;
        foreground_color_value.Visible = true;
        Change_Form_Colour_Button.Visible = true;
    }
    private void Change_Form_Colour_Button_Click(object? sender, EventArgs e)
    {
        if (background_color_value.Text.Equals(foreground_color_value.Text))
            MessageBox.Show("Colours cannot be same", "Error");
        else
        {
            color_background = (byte)background_color_value.SelectedIndex;
            color_foreground = (byte)foreground_color_value.SelectedIndex;
            SetFormColor();

            BinaryWriter binWriter = new(File.Open("settings.dat", FileMode.Open));
            binWriter.Seek(sizeof(int), SeekOrigin.Begin);
            binWriter.Write(color_background);
            binWriter.Write(color_foreground);
            binWriter.Close();

            Encryption("settings.dat", Decision.ENCRYPT, encryptSettings);
            BackUp("settings.dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption("settings.dat", Decision.DECRYPT, encryptSettings);

            MessageBox.Show("Colours are changed !!!", "Message");
        }
    }

    private void Delete_a_User_Click(object? sender, EventArgs e)
    {
        if (no_of_users == 0)
            MessageBox.Show("No user available", "Notice");
        else
        {
            Welcome.Visible = false;
            Log_In.Visible = false;
            Register_a_User.Visible = false;
            Change_Form_Colour.Visible = false;
            Delete_a_User.Visible = false;
            Quit.Visible = false;

            Back_To_Main_Menu.Visible = true;
            verify_login_id.Visible = true;
            verify_login_id_value.Text = "";
            verify_login_id_value.Visible = true;
            verify_login_username.Visible = true;
            verify_login_username_value.Text = "";
            verify_login_username_value.Visible = true;
            verify_login_password.Visible = true;
            verify_login_password_value.Text = "";
            verify_login_password_value.Visible = true;
            Delete_Button.Visible = true;
        }
    }
    private void Delete_Button_Click(object sender, EventArgs e)
    {
        if (!File.Exists(verify_login_id_value.Text + ".dat"))
        {
            MessageBox.Show("ID doesn't exist", "Error");
            return;
        }
        BinaryReader binReader = new(File.Open("settings.dat", FileMode.Open));
        binReader.ReadInt32();
        binReader.ReadByte();
        binReader.ReadByte();
        for (int i = 0; i < no_of_users; i++)
        {
            int desiredID = binReader.ReadInt32();
            if (desiredID == Convert.ToInt32(verify_login_id_value.Text))
            {
                for (int j = 0; j < 256; j++)
                    encryptionCode[j] = binReader.ReadByte();
                break;
            }
            else
            {
                for (int j = 0; j < 256; j++)
                    binReader.ReadByte();
            }
        }
        binReader.Close();

        Encryption(verify_login_id_value.Text + ".dat", Decision.DECRYPT, encryptionCode);
        binReader = new(File.Open(verify_login_id_value.Text + ".dat", FileMode.Open));
        string user = binReader.ReadString();
        string pass = binReader.ReadString();
        binReader.Close();
        Encryption(verify_login_id_value.Text + ".dat", Decision.ENCRYPT, encryptionCode);
        if (!user.Equals(verify_login_username_value.Text) || !pass.Equals(verify_login_password_value.Text))
            MessageBox.Show("Both username & password must be correct", "Error");
        else
        {
            BackUp(verify_login_id_value.Text, Decision.REMOVE_THE_BACKUP_FILE);
            File.Delete(verify_login_id_value.Text + ".dat");
            no_of_users--;
            binReader = new(File.Open("settings.dat", FileMode.Open));
            BinaryWriter binWriter = new(File.Open("temp.dat", FileMode.Create));
            binReader.ReadInt32();
            binWriter.Write(no_of_users);
            binWriter.Write(binReader.ReadByte());
            binWriter.Write(binReader.ReadByte());
            for (int i = 0; i <= no_of_users; i++)
            {
                int desiredID = binReader.ReadInt32();
                if (desiredID == Convert.ToInt32(verify_login_id_value.Text))
                {
                    for (int j = 0; j < 256; j++)
                        binReader.ReadByte();
                }
                else
                {
                    binWriter.Write(desiredID);
                    for (int j = 0; j < 256; j++)
                        binWriter.Write(binReader.ReadByte());

                }
            }
            binWriter.Close();
            binReader.Close();

            File.Delete("settings.dat");
            File.Move("temp.dat", "settings.dat");

            Encryption("settings.dat", Decision.ENCRYPT, encryptSettings);
            BackUp("settings.dat", Decision.MAKE_THE_BACKUP_FILE);
            Encryption("settings.dat", Decision.DECRYPT, encryptSettings);

            MessageBox.Show("ID is deleted !!!", "Message");
            verify_login_id_value.Text = "";
            verify_login_username_value.Text = "";
            verify_login_password_value.Text = "";

            if (no_of_users == 0)
                Back_To_Main_Menu.PerformClick();
        }
    }

    private void Quit_Click(object? sender, EventArgs e)
    {
        Welcome.Visible = false;
        Log_In.Visible = false;
        Register_a_User.Visible = false;
        Change_Form_Colour.Visible = false;
        Delete_a_User.Visible = false;
        Quit.Visible = false;

        GoodBye.Visible = true;
        BackUp("settings.dat", Decision.REMOVE_THE_BACKUP_FILE);
        Encryption("settings.dat", Decision.ENCRYPT, encryptSettings);
    }
}
