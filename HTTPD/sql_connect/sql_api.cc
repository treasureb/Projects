#include"sql_api.h"

using namespace std;

sqlApi::sqlApi(const string& _h,\
               const int& _port,\
               const string& _u,\
               const string& _p,\
               const string& _db)
{
    host = _h;
    user = _u;
    passwd = _p;
    db = _db;
    port = _port;
    res = NULL;
    conn = mysql_init(NULL);
}

int sqlApi::connect()
{
    if(mysql_real_connect(conn,host.c_str(),user.c_str(),"",db.c_str(),port,NULL,0)){
        cout<<"connect success!"<<endl;
        }else{
        cout<<"connect error!"<<endl;
        }
}

//name sex age hobby school
int sqlApi::insert(const string& _name,\
           const string& _sex,\
           const string& _age,\
           const string& _hobby,\
           const string& _school)
{
    string sql = "insert into student (name,sex,age,hobby,school) values('";
    sql += _name;
    sql += "','";
    sql += _sex;
    sql += "','";
    sql += _age;
    sql += "','";
    sql += _hobby;
    sql += "','";
    sql += _school;
    sql += "')";

    cout<<sql<<endl;
    int ret = mysql_query(conn,sql.c_str());
    cout<<"ret:  "<<ret<<endl;
    return ret;
}

int sqlApi::select()
{
    string sql = "select * from student";
    if(mysql_query(conn,sql.c_str()) == 0){
        res = mysql_store_result(conn);
        if(res){
            int nums = mysql_num_rows(res);
            int col  = mysql_num_fields(res);
            cout<<"nums: "<<nums<<"cols: "<<col<<endl;
            MYSQL_FIELD *fd;
            for(; fd = mysql_fetch_field(res);){
                cout<<fd->name<<" ";
            }
            cout<<endl;
        
            int i = 0;
            int j = 0;
            for(; i < nums;i++){
                MYSQL_ROW row_res = mysql_fetch_row(res);
                j = 0;
                for(; j < col;j++){
                    cout<<row_res[j]<<' ';
                }
                cout<<endl;
            }
            cout<<endl;
        }
        free(res);
    }
}

sqlApi::~sqlApi()
{
    mysql_close(conn);
}
