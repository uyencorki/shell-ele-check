#include "support_function.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>








//CONVERT
//########################################################################################################
string sf_string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}


long sf_hex_to_int(char ch)
{

    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

//change 0x32 as char to 0x32 as hex, return long
long sf_char_hex_to_int(const char *input_char){
    int i,  trongso = 1;
    long value = 0;
    for(i = strlen(input_char)-1; i >= 0; i--){

        //loai bo ky tu x
        if(input_char[i]=='x') continue;

        //loai bo space
//		if(input_char[i]==' ') continue;

        if(sf_hex_to_int(input_char[i])==-1) {
            printf("data hex input error %s\n", input_char);
            return -1;
        }
        value +=  sf_hex_to_int(input_char[i])*trongso;
        trongso = trongso*16;
    }
    return value;
}
//########################################################################################################








//FILE
//########################################################################################################
int sf_filesize(QString filename)
{
//    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
//    return in.tellg();
    int size = 0;
    QFile myFile(filename);
    if (myFile.open(QIODevice::ReadOnly))
    {
        size = myFile.size();  //when file does open.
        myFile.close();
    }
    return size;

}

/*
return 1 if found
return -1 if not found

tu do dai cua output co the suy ra duoc vi tri cua no ben trong input_char
*/
int sf_find_data_after_key_in_string(string input_char, string key, string &output)
{
    int key_len = key.length(); //strlen(key);

    bool find_ok = false;

    //find
    for (int i=0; i<(int)input_char.length(); i++)
    {
        if(input_char.compare(i, key_len, key.c_str(), key_len)==0)
        {
            output = input_char.substr(i + key_len);

            //tim thay 1 OK roi thi out luon
            find_ok = true;
            break;
        }
    }

    if(find_ok==false) return -1;

    return 1;
}

/*
tim tat ca cac line ben trong file
return line position if found
return -1 if not found

loai bo 0x0a 0x0d o cuoi cung
*/
int sf_find_data_after_key_in_file_linebyline(QString filename, string key, string &output)
{

    QFile file(filename);
    qDebug() << "filename = "  <<filename <<  endl;
    bool find_ok = false;
    int line_num = 0;
    string data_line;


    if (file.open(QIODevice::ReadOnly))
    {
       QTextStream in(&file);
       while (!in.atEnd())
       {
          QString line = in.readLine();
//          cout << "line = " << line.toStdString() << endl;
          data_line = line.toStdString();

          if(sf_find_data_after_key_in_string(data_line, key, output)==1)
          {
            find_ok = true;
            break;
          }

            line_num++;
        }

       file.close();
    }
    else
    {
        cout << "can't open file 111 " << endl;
    }


//    if(file.is_open())
//    {
//        while((!file.eof())&&(find_ok==false))
//        {
//            getline(file, data_line);
//            //find
//            if(sf_find_data_after_key_in_string(data_line, key, output)==1)
//            {

////                string test = "no text";
////                find_data_between_2_key_in_string(data_line, "\"", "\"", test);

//                find_ok = true;
//                break;
//            }

//            line_num++;
//        }
//        file.close();
//    }
//    else
//    {
//        cout << "can't open file" <<endl;
//    }





    if(find_ok==false) line_num = -1;

    return line_num;
}



/*
return position if found
return -1 if not found

co pos va do dai cua output -> no nam o dau ben trong input_char
*/


int sf_find_data_between_2_key_in_string(string input_char, string start_key, string end_key, string &output)
{
    bool find_ok = false;

    int pos1 = 0, pos2 = 0;

    //find
    bool start_ok = false; //-> tim lai tu dau
    for(unsigned int i=0; i< input_char.length(); i++){
        //tim hn_start
        if(start_ok==false){
            if(input_char.compare(i, start_key.length(), start_key)==0){
                start_ok = true;
                pos1 = i + start_key.length();
//                cout << "1 : " << input.substr(pos1) << endl;


                i+=start_key.length();
                continue;
            }
        }

        //tim hn_end
        if(start_ok==true){
            if(input_char.compare(i, end_key.length(), end_key)==0){
                start_ok = true;
                pos2 = i;

                //result
                output = input_char.substr(pos1, pos2 - pos1);
                //show

//                cout << pos1 << " - " << pos2 << endl;
//                cout << "out = " << output << endl;


                //set false de tim nhung phan tu tiep theo
//                start_ok = false;
                break; // tim 1 lan thoi
            }
        }
    }



    if(find_ok==false) return -1;

    return 1;
}

/*
return 1 if change OK
return -1 if not OK
*/


int sf_change_data_line_in_file(QString filename, int line_num_change, string data_change)
{
    QFile file(filename);
    qDebug() << "filename = "  <<filename <<  endl;
    bool find_ok = false;
    int line_num = 0;
    string data_line;
    vector<string> data_lines;


    if (file.open(QIODevice::ReadWrite))
    {
       QTextStream in(&file);
       while (!in.atEnd())
       {
          QString line = in.readLine();
//          cout << "line = " << line.toStdString() << endl;
          data_line = line.toStdString();

          if(line_num==line_num_change) data_line = data_change;
          //save lai
          data_lines.push_back(data_line);

          line_num++;
        }

       file.close();
    }
    else
    {
        cout << "can't open file 111 " << endl;
    }


   // save data

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 0;
    QTextStream out(&file);
    for(int i=0; i<line_num;i++)
    {
//      if(i!=line_num-1) file_save << data_lines[i] << endl;
//      else file_save << data_lines[i];

        QString Q_data = QString::fromStdString(data_lines[i]);

        if (i!=line_num-1) out << Q_data << endl;
        else out << Q_data << endl;
    }

    file.close();


//    file.open(QIODevice::ReadWrite);
//    for(int i=0; i<line_num;i++)
//    {
//        if(i!=line_num-1) file_save << data_lines[i] << endl;
//        else file_save << data_lines[i];
//    }

//    file.close();


    //release data
    vector<string>().swap(data_lines);


    return 1;

exit:
    return -1;




//    ifstream file_open;
//    ofstream file_save;

//    file_open.open(filename, ios_base::in | ios_base::out);

//    vector<string> data_lines;

////    cout << "line_num_change = " << line_num_change << endl;

//    int line_num = 0;
//    string data_line;
//    if(file_open.is_open())
//    {
//        while((!file_open.eof()))
//        {
//            getline(file_open, data_line);

//            if(line_num==line_num_change) data_line = data_change;


//            //save lai
//            data_lines.push_back(data_line);
//            line_num++;
//        }

//        file_open.close();
//    }
//    else
//    {
//        cout << "can't open file" <<endl;
//        goto exit;
//    }


//    //save
//    file_save.open(filename, ios_base::trunc | ios_base::out);
//    for(int i=0; i<line_num;i++)
//    {
//        if(i!=line_num-1) file_save << data_lines[i] << endl;
//        else file_save << data_lines[i];
//    }

//    file_save.close();


//    //release data
//    vector<string>().swap(data_lines);


//    return 1;

//exit:
//    return -1;


}


//########################################################################################################
