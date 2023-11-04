namespace The_Diary;

partial class Diary
{
    /// <summary>
    ///  Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    ///  Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null))
        {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    ///  Required method for Designer support - do not modify
    ///  the contents of this method with the code editor.
    /// </summary>

    private System.Windows.Forms.Button Back_To_Main_Menu;
    private System.Windows.Forms.Label Welcome;

    private System.Windows.Forms.Button Log_In;
    private System.Windows.Forms.Label login_id;
    private System.Windows.Forms.TextBox login_id_value;
    private System.Windows.Forms.Label login_username;
    private System.Windows.Forms.TextBox login_username_value;
    private System.Windows.Forms.Label login_password;
    private System.Windows.Forms.TextBox login_password_value;
    private System.Windows.Forms.Button Login_Button;

    private System.Windows.Forms.Button Register_a_User;
    private System.Windows.Forms.Label new_username;
    private System.Windows.Forms.TextBox new_username_value;
    private System.Windows.Forms.Label new_password;
    private System.Windows.Forms.TextBox new_password_value;
    private System.Windows.Forms.Label reEnter_new_password;
    private System.Windows.Forms.TextBox reEnter_new_password_value;
    private System.Windows.Forms.Button Register_Button;


    private System.Windows.Forms.Button Change_Form_Colour;
    private System.Windows.Forms.Label background_color;
    private System.Windows.Forms.ComboBox background_color_value;
    private System.Windows.Forms.Label foreground_color;
    private System.Windows.Forms.ComboBox foreground_color_value;
    private System.Windows.Forms.Button Change_Form_Colour_Button;

    private System.Windows.Forms.Button Delete_a_User;
    private System.Windows.Forms.Label verify_login_id;
    private System.Windows.Forms.TextBox verify_login_id_value;
    private System.Windows.Forms.Label verify_login_username;
    private System.Windows.Forms.TextBox verify_login_username_value;
    private System.Windows.Forms.Label verify_login_password;
    private System.Windows.Forms.TextBox verify_login_password_value;
    private System.Windows.Forms.Button Delete_Button;

    private System.Windows.Forms.Button Quit;
    private System.Windows.Forms.Label GoodBye;

    private void InitializeComponent()
    {
        this.Back_To_Main_Menu = new System.Windows.Forms.Button();
        this.Welcome = new System.Windows.Forms.Label();

        this.Log_In = new System.Windows.Forms.Button();
        this.login_id = new System.Windows.Forms.Label();
        this.login_id_value = new System.Windows.Forms.TextBox();
        this.login_username = new System.Windows.Forms.Label();
        this.login_username_value = new System.Windows.Forms.TextBox();
        this.login_password = new System.Windows.Forms.Label();
        this.login_password_value = new System.Windows.Forms.TextBox();
        this.Login_Button = new System.Windows.Forms.Button();

        this.Register_a_User = new System.Windows.Forms.Button();
        this.new_username = new System.Windows.Forms.Label();
        this.new_username_value = new System.Windows.Forms.TextBox();
        this.new_password = new System.Windows.Forms.Label();
        this.new_password_value = new System.Windows.Forms.TextBox();
        this.reEnter_new_password = new System.Windows.Forms.Label();
        this.reEnter_new_password_value = new System.Windows.Forms.TextBox();
        this.Register_Button = new System.Windows.Forms.Button();

        this.Change_Form_Colour = new System.Windows.Forms.Button();
        this.background_color = new System.Windows.Forms.Label();
        this.background_color_value = new System.Windows.Forms.ComboBox();
        this.foreground_color = new System.Windows.Forms.Label();
        this.foreground_color_value = new System.Windows.Forms.ComboBox();
        this.Change_Form_Colour_Button = new System.Windows.Forms.Button();

        this.Delete_a_User = new System.Windows.Forms.Button();
        this.verify_login_id = new System.Windows.Forms.Label();
        this.verify_login_id_value = new System.Windows.Forms.TextBox();
        this.verify_login_username = new System.Windows.Forms.Label();
        this.verify_login_username_value = new System.Windows.Forms.TextBox();
        this.verify_login_password = new System.Windows.Forms.Label();
        this.verify_login_password_value = new System.Windows.Forms.TextBox();
        this.Delete_Button = new System.Windows.Forms.Button();

        this.Quit = new System.Windows.Forms.Button();
        this.GoodBye = new System.Windows.Forms.Label();

        this.SuspendLayout();

        //
        // Back To Main Menu
        //
        this.Back_To_Main_Menu.AutoSize = true;
        this.Back_To_Main_Menu.Text = "Go Back";
        this.Back_To_Main_Menu.Location = new System.Drawing.Point(20, 10);
        this.Back_To_Main_Menu.Visible = false;
        this.Back_To_Main_Menu.Click += new EventHandler((object sender, EventArgs e) =>
        {
            foreach (Control c in Controls)
            {
                if (c.Name.Equals("0") || c.Name.Equals("1") || c.Name.Equals("2") || c.Name.Equals("3") || c.Name.Equals("4") || c.Name.Equals("5"))
                    c.Visible = true;
                else
                    c.Visible = false;
            }
        });
        //
        // Welcome
        //
        this.Welcome.Name = "0";
        this.Welcome.AutoSize = true;
        this.Welcome.Text = "Welcome To Diary!!!!!";
        this.Welcome.Font = new System.Drawing.Font("Segoe UI", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
        this.Welcome.Location = new System.Drawing.Point(180, 30);
        this.Welcome.Visible = true;

        //
        // Log_In
        //
        this.Log_In.Name = "1";
        this.Log_In.Text = "Log In";
        this.Log_In.Location = new System.Drawing.Point(240, 120);
        this.Log_In.Size = new System.Drawing.Size(140, 25);
        this.Log_In.Click += new System.EventHandler(Log_In_Click);
        this.Log_In.Visible = true;
        //
        // login_id
        //
        this.login_id.AutoSize = true;
        this.login_id.Text = "Enter ID";
        this.login_id.Location = new System.Drawing.Point(180, 120);
        this.login_id.Visible = false;
        //
        // login_id_value
        //
        this.login_id_value.Location = new System.Drawing.Point(300, 117);
        this.login_id_value.Size = new System.Drawing.Size(140, 25);
        this.login_id_value.MaxLength = 5;
        this.login_id_value.Visible = false;
        //
        //
        // login_username
        //
        this.login_username.AutoSize = true;
        this.login_username.Text = "Enter Username";
        this.login_username.Location = new System.Drawing.Point(180, 170);
        this.login_username.Visible = false;
        //
        // login_username_value
        //
        this.login_username_value.Location = new System.Drawing.Point(300, 167);
        this.login_username_value.Size = new System.Drawing.Size(140, 25);
        this.login_username_value.MaxLength = 20;
        this.login_username_value.Visible = false;
        //
        // login_password
        //
        this.login_password.AutoSize = true;
        this.login_password.Text = "Enter Password";
        this.login_password.Location = new System.Drawing.Point(180, 220);
        this.login_password.Visible = false;
        //
        // login_password_value
        //
        this.login_password_value.Location = new System.Drawing.Point(300, 217);
        this.login_password_value.Size = new System.Drawing.Size(140, 25);
        this.login_password_value.MaxLength = 20;
        this.login_password_value.PasswordChar = '*';
        this.login_password_value.Visible = false;
        //
        // Login_Button
        //
        this.Login_Button.Text = "Login";
        this.Login_Button.Location = new System.Drawing.Point(235, 270);
        this.Login_Button.Size = new System.Drawing.Size(140, 25);
        this.Login_Button.Click += new System.EventHandler(Login_Button_Click);
        this.Login_Button.Visible = false;

        //
        // Register_a_User
        //
        this.Register_a_User.Name = "2";
        this.Register_a_User.Text = "Register a User";
        this.Register_a_User.Location = new System.Drawing.Point(240, 160);
        this.Register_a_User.Size = new System.Drawing.Size(140, 25);
        this.Register_a_User.Click += new System.EventHandler(Register_a_User_Click);
        this.Register_a_User.Visible = true;
        //
        // new_username
        //
        this.new_username.AutoSize = true;
        this.new_username.Text = "Enter New Username";
        this.new_username.Location = new System.Drawing.Point(180, 120);
        this.new_username.Visible = false;
        //
        // new_username_value
        //
        this.new_username_value.Location = new System.Drawing.Point(300, 117);
        this.new_username_value.Size = new System.Drawing.Size(140, 25);
        this.new_username_value.MaxLength = 20;
        this.new_username_value.Visible = false;
        //
        // new_password
        //
        this.new_password.AutoSize = true;
        this.new_password.Text = "Enter New Password";
        this.new_password.Location = new System.Drawing.Point(180, 170);
        this.new_password.Visible = false;
        //
        // new_password_value
        //
        this.new_password_value.Location = new System.Drawing.Point(300, 167);
        this.new_password_value.Size = new System.Drawing.Size(140, 25);
        this.new_password_value.MaxLength = 20;
        this.new_password_value.PasswordChar = '*';
        this.new_password_value.Visible = false;
        //
        // reEnter_new_password
        //
        this.reEnter_new_password.AutoSize = true;
        this.reEnter_new_password.Text = "Re-Enter New Password";
        this.reEnter_new_password.Location = new System.Drawing.Point(160, 220);
        this.reEnter_new_password.Visible = false;
        //
        // reEnter_new_password_value
        //
        this.reEnter_new_password_value.Location = new System.Drawing.Point(300, 217);
        this.reEnter_new_password_value.Size = new System.Drawing.Size(140, 25);
        this.reEnter_new_password_value.MaxLength = 20;
        this.reEnter_new_password_value.PasswordChar = '*';
        this.reEnter_new_password_value.Visible = false;
        //
        // Register_Button
        //
        this.Register_Button.Text = "Register";
        this.Register_Button.Location = new System.Drawing.Point(235, 270);
        this.Register_Button.Size = new System.Drawing.Size(140, 25);
        this.Register_Button.Click += new System.EventHandler(Register_Button_Click);
        this.Register_Button.Visible = false;

        //
        // Change_Form_Colour
        //
        this.Change_Form_Colour.Name = "3";
        this.Change_Form_Colour.Text = "Change Form Colour";
        this.Change_Form_Colour.Location = new System.Drawing.Point(240, 200);
        this.Change_Form_Colour.Size = new System.Drawing.Size(140, 25);
        this.Change_Form_Colour.Click += new System.EventHandler(Change_Form_Colour_Click);
        this.Change_Form_Colour.Visible = true;
        //
        // background_color
        //
        this.background_color.Text = "Background Colour";
        this.background_color.Location = new System.Drawing.Point(50, 100);
        this.background_color.Size = new System.Drawing.Size(100, 100);
        this.background_color.Visible = false;
        //
        // background_color_value
        //
        this.background_color_value.Location = new System.Drawing.Point(250, 100);
        this.background_color_value.Size = new System.Drawing.Size(100, 100);
        this.background_color_value.Items.Add("Grey");
        this.background_color_value.Items.Add("Black");
        this.background_color_value.Items.Add("White");
        this.background_color_value.Items.Add("Red");
        this.background_color_value.Items.Add("Green");
        this.background_color_value.Items.Add("Blue");
        this.background_color_value.Items.Add("Yellow");
        this.background_color_value.Visible = false;
        //
        // foreground_color
        //
        this.foreground_color.Text = "Foreground Colour";
        this.foreground_color.Location = new System.Drawing.Point(50, 200);
        this.foreground_color.Size = new System.Drawing.Size(100, 100);
        this.foreground_color.Visible = false;
        //
        // foreground_color_value
        //
        this.foreground_color_value.Location = new System.Drawing.Point(250, 200);
        this.foreground_color_value.Size = new System.Drawing.Size(100, 100);
        this.foreground_color_value.Items.Add("Grey");
        this.foreground_color_value.Items.Add("Black");
        this.foreground_color_value.Items.Add("White");
        this.foreground_color_value.Items.Add("Red");
        this.foreground_color_value.Items.Add("Green");
        this.foreground_color_value.Items.Add("Blue");
        this.foreground_color_value.Items.Add("Yellow");
        this.foreground_color_value.Visible = false;
        //
        // Change_Form_Colour_Button
        //
        this.Change_Form_Colour_Button.Text = "Change";
        this.Change_Form_Colour_Button.Location = new System.Drawing.Point(235, 300);
        this.Change_Form_Colour_Button.Size = new System.Drawing.Size(140, 25);
        this.Change_Form_Colour_Button.Click += new System.EventHandler(Change_Form_Colour_Button_Click);
        this.Change_Form_Colour_Button.Visible = false;

        //
        // Delete_a_User
        //
        this.Delete_a_User.Name = "4";
        this.Delete_a_User.Text = "Delete a User";
        this.Delete_a_User.Location = new System.Drawing.Point(240, 240);
        this.Delete_a_User.Size = new System.Drawing.Size(140, 25);
        this.Delete_a_User.Click += new System.EventHandler(Delete_a_User_Click);
        this.Delete_a_User.Visible = true;
        //
        // verify_login_id
        //
        this.verify_login_id.AutoSize = true;
        this.verify_login_id.Text = "Confirm ID";
        this.verify_login_id.Location = new System.Drawing.Point(180, 120);
        this.verify_login_id.Visible = false;
        //
        // verify_login_id_value
        //
        this.verify_login_id_value.Location = new System.Drawing.Point(300, 117);
        this.verify_login_id_value.Size = new System.Drawing.Size(140, 25);
        this.verify_login_id_value.MaxLength = 5;
        this.verify_login_id_value.Visible = false;
        //
        // verify_login_username
        //
        this.verify_login_username.AutoSize = true;
        this.verify_login_username.Text = "Confirm Username";
        this.verify_login_username.Location = new System.Drawing.Point(180, 170);
        this.verify_login_username.Visible = false;
        //
        // verify_login_username_value
        //
        this.verify_login_username_value.Location = new System.Drawing.Point(300, 167);
        this.verify_login_username_value.Size = new System.Drawing.Size(140, 25);
        this.verify_login_username_value.MaxLength = 20;
        this.verify_login_username_value.Visible = false;
        //
        // verify_login_password
        //
        this.verify_login_password.AutoSize = true;
        this.verify_login_password.Text = "Confirm Password";
        this.verify_login_password.Location = new System.Drawing.Point(180, 220);
        this.verify_login_password.Visible = false;
        //
        // verify_login_password_value
        //
        this.verify_login_password_value.Location = new System.Drawing.Point(300, 217);
        this.verify_login_password_value.Size = new System.Drawing.Size(140, 25);
        this.verify_login_password_value.MaxLength = 20;
        this.verify_login_password_value.PasswordChar = '*';
        this.verify_login_password_value.Visible = false;
        //
        // Delete_Button
        //
        this.Delete_Button.Text = "Delete";
        this.Delete_Button.Location = new System.Drawing.Point(235, 270);
        this.Delete_Button.Size = new System.Drawing.Size(140, 25);
        this.Delete_Button.Click += new System.EventHandler(Delete_Button_Click);
        this.Delete_Button.Visible = false;

        //
        // Quit
        //
        this.Quit.Name = "5";
        this.Quit.Text = "Quit";
        this.Quit.Location = new System.Drawing.Point(240, 280);
        this.Quit.Size = new System.Drawing.Size(140, 25);
        this.Quit.Click += new System.EventHandler(Quit_Click);
        this.Quit.Visible = true;
        //
        // GoodBye
        //
        this.GoodBye.AutoSize = true;
        this.GoodBye.Text = "See you next time !!!";
        this.GoodBye.Font = new System.Drawing.Font("Segoe UI", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
        this.GoodBye.Location = new System.Drawing.Point(190, 180);
        this.GoodBye.Visible = false;

        //
        // form
        //
        this.Size = new System.Drawing.Size(640, 480);
        this.Location = new System.Drawing.Point(0, 0);
        this.Text = "Diary in C#";

        this.Controls.Add(this.Back_To_Main_Menu);
        this.Controls.Add(this.Welcome);

        this.Controls.Add(this.Log_In);
        this.Controls.Add(this.login_id);
        this.Controls.Add(this.login_id_value);
        this.Controls.Add(this.login_username);
        this.Controls.Add(this.login_username_value);
        this.Controls.Add(this.login_password);
        this.Controls.Add(this.login_password_value);
        this.Controls.Add(this.Login_Button);

        this.Controls.Add(this.Register_a_User);
        this.Controls.Add(this.new_username);
        this.Controls.Add(this.new_username_value);
        this.Controls.Add(this.new_password);
        this.Controls.Add(this.new_password_value);
        this.Controls.Add(this.reEnter_new_password);
        this.Controls.Add(this.reEnter_new_password_value);
        this.Controls.Add(this.Register_Button);

        this.Controls.Add(this.Change_Form_Colour);
        this.Controls.Add(this.background_color);
        this.Controls.Add(this.background_color_value);
        this.Controls.Add(this.foreground_color);
        this.Controls.Add(this.foreground_color_value);
        this.Controls.Add(this.Change_Form_Colour_Button);

        this.Controls.Add(this.Delete_a_User);
        this.Controls.Add(this.verify_login_id);
        this.Controls.Add(this.verify_login_id_value);
        this.Controls.Add(this.verify_login_username);
        this.Controls.Add(this.verify_login_username_value);
        this.Controls.Add(this.verify_login_password);
        this.Controls.Add(this.verify_login_password_value);
        this.Controls.Add(this.Delete_Button);

        this.Controls.Add(this.Quit);
        this.Controls.Add(this.GoodBye);

        this.ResumeLayout(false);
    }

    #endregion
}
