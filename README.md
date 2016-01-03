# easydb
A simple key-value database  
On linus platform  
Only single process & single thread  
Real-time write to disk but poor read, especially for backup system  

Example Code:    
    DB *p_db = NULL;
    Options opt;
    //opt.max_log_file_size = 100;
    //opt.file_sync_interval = 1;
    Status s = DB::Open(opt, "./db_test", &p_db);

    if(NULL == p_db)
    {
        printf("open db error|%s\n", s.ToString().c_str());
        return 0;
    }

    for(int i = 0; i < 10; ++i)
    {
        char buff[16] = {0};
        snprintf(buff, sizeof(buff), "%d", i);
        s = p_db->Put(WriteOptions(), buff, "hello,world");
        if(!s.ok())
        {
            printf("put error|%s\n", s.ToString().c_str());
            return 0;
        }
    }

    std::map<std::string, std::string> map_all_kv;
    s = p_db->GetAll(map_all_kv);

    std::map<std::string, std::string>::iterator iter;
    for(iter = map_all_kv.begin(); iter != map_all_kv.end(); ++iter)
    {
        printf("%s->%s\n", iter->first.c_str(), iter->second.c_str());
    }

    delete p_db;


