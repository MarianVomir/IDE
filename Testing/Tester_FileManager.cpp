#include "Tester_FileManager.h"

Tester_FileManager::Tester_FileManager()
{
    //Set up
    home = QDir::homePath() + "/";
}

Tester_FileManager::~Tester_FileManager()
{
    //Tear down
    FileManager::DeleteDir(home + "mydir");
    FileManager::DeleteDir(home + "mydir2");
    QFile(home + "myfile").setPermissions(QFile::WriteUser); // set the permission to be able to delete
    FileManager::DeleteFile(home + "myfile");
    FileManager::DeleteFile(home + "otherfile");
    FileManager::DeleteFile(home + "abcdexists");
    QFile(home + "abcdirexists/").setPermissions(QFile::WriteUser | QFile::ReadUser | QFile::ExeUser); // set the permission to be able to delete
    FileManager::DeleteDir(home + "abcdirexists/");
}

void Tester_FileManager::Test()
{
    // CreateFile

        // base case
        FileManager::CreateFile(home + "myfile");
        Q_ASSERT(QFile(home + "myfile").exists());

        // already exists
        try
        {
            FileManager::CreateFile(home + "myfile");
            Q_ASSERT(false);
        }
        catch (AlreadyExistsException& ex)
        {
            Q_ASSERT(true);
        }

        // no permission
        try
        {
            FileManager::CreateFile("/bin/hello");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // invalid filename - / at end of filename
        try
        {
            FileManager::CreateFile(home + "abc/");
            Q_ASSERT(false);
        }
        catch (InvalidPathException& ex)
        {
            Q_ASSERT(true);
        }

    // DeleteFile

        // base case - existent file
        FileManager::DeleteFile(home + "myfile");
        Q_ASSERT(QFile(home + "myfile").exists() == false);

        // base case - inexistent file, must not throw exceptions
        Q_ASSERT(QFile(home + "inexistentfile").exists() == false);
        FileManager::DeleteFile(home + "inexistentfile");
        Q_ASSERT(QFile(home + "inexistentfile").exists() == false);

        // no permission
        try
        {
            FileManager::DeleteFile("/bin/rm");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // invalid filename - / at end of filename
        try
        {
            FileManager::DeleteFile(home + "abc/");
            Q_ASSERT(false);
        }
        catch (InvalidPathException& ex)
        {
            Q_ASSERT(true);
        }

    // CreateDir

        // base case - without slash at end
        FileManager::CreateDir(home + "mydir");
        Q_ASSERT(QDir(home + "mydir").exists());
        Q_ASSERT(QDir(home + "mydir/").exists());

        // base case 2 - with slash at end
        FileManager::CreateDir(home + "mydir2/");
        Q_ASSERT(QDir(home + "mydir2").exists());
        Q_ASSERT(QDir(home + "mydir2/").exists());

        // already exists
        try
        {
            FileManager::CreateDir(home + "mydir");
            Q_ASSERT(false);
        }
        catch (AlreadyExistsException& ex)
        {
            Q_ASSERT(true);
        }

        // no permission
        try
        {
            FileManager::CreateDir("/bin/hello");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // no permission 2 - must fail to create "root"
        try
        {
            FileManager::CreateDir("/");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // create nested dirs with files in them
        FileManager::CreateDir(home + "nested1/nested2");
        FileManager::CreateDir(home + "nested1/nested3");
        FileManager::CreateDir(home + "nested2/nested4");
        FileManager::CreateFile(home + "nested1/nested2/abc.txt");
        FileManager::CreateFile(home + "nested1/nested3/abcdef.txt");
        FileManager::CreateFile(home + "nested1/nested4/abcdefgh.txt");
        FileManager::CreateFile(home + "nested1/nested5/nested6/nested7/abcdefgh.txt");
        Q_ASSERT(QFileInfo(home + "nested1").exists());
        Q_ASSERT(QFileInfo(home + "nested1/nested2").exists());
        Q_ASSERT(QFileInfo(home + "nested1/nested3").exists());
        Q_ASSERT(QFileInfo(home + "nested1/nested4").exists());
        Q_ASSERT(QFileInfo(home + "nested1/nested5/nested6/nested7/").exists());


    // DeleteDir

        // base case - dir exists
        FileManager::DeleteDir(home + "mydir/");
        Q_ASSERT(QDir(home + "mydir/").exists() == false);
        FileManager::DeleteDir(home + "mydir2");
        Q_ASSERT(QDir(home + "mydir2/").exists() == false);

        // base case - dir doesn't exist, must not throw exceptions
        Q_ASSERT(QDir(home + "mydir/").exists() == false);
        FileManager::DeleteDir(home + "mydir/");
        Q_ASSERT(QDir(home + "mydir/").exists() == false);

        // no permission
        try
        {
            FileManager::DeleteDir("/etc/hosts");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // no permission 2 - must fail to delete "root"
        try
        {
            FileManager::DeleteDir("/");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // delete nested non-empty dir
        FileManager::DeleteDir(home + "nested1");
        Q_ASSERT(QFileInfo(home + "nested1").exists() == false);
        FileManager::DeleteDir(home + "nested2/nested4");
        Q_ASSERT(QFileInfo(home + "nested2/nested4").exists() == false);
        Q_ASSERT(QFileInfo(home + "nested2").exists());
        FileManager::DeleteDir(home + "nested2");
        Q_ASSERT(QFileInfo(home + "nested2").exists() == false);

    // WriteFile

        // base case - existent file
        FileManager::CreateFile(home + "myfile");
        FileManager::WriteFile(home + "myfile", "Hello World, this was written from the FileManager");

        // base case - inexistent file
        FileManager::WriteFile(home + "otherfile", "Hello World, this file was created out of nowhere by the FileManager");

        // no permissions - existent file
        try
        {
            FileManager::WriteFile("/bin/rm", "this should not be written");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

        // no permissions - inexistent file
        try
        {
            FileManager::WriteFile("/myinexistentfile", "this should not be written");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }


    // ReadFile
        // base case - existent file
        Q_ASSERT(FileManager::ReadFile(home + "myfile") == "Hello World, this was written from the FileManager");
        Q_ASSERT(FileManager::ReadFile(home + "otherfile") == "Hello World, this file was created out of nowhere by the FileManager");

        // base case - inexistent file
        try
        {
            FileManager::ReadFile(home + "inexistentfile");
            Q_ASSERT(false);
        }
        catch (NotFoundException& ex)
        {
            Q_ASSERT(true);
        }

        QFile(home+"myfile").setPermissions(0);
        // no permissions
        try
        {

            FileManager::ReadFile(home+"myfile");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException& ex)
        {
            Q_ASSERT(true);
        }

    // Exists

        // existent file
        FileManager::CreateFile(home + "abcdexists");
        Q_ASSERT(FileManager::Exists(home + "abcdexists"));
        Q_ASSERT(FileManager::Exists(home + "abcdexists/") == false);

        // existent dir
        FileManager::CreateDir(home + "abcdirexists");
        Q_ASSERT(FileManager::Exists(home + "abcdirexists"));
        Q_ASSERT(FileManager::Exists(home + "abcdirexists/"));

        // inexistent file
        Q_ASSERT(FileManager::Exists(home + "aaaaa") == false);

        // inexistent dir
        Q_ASSERT(FileManager::Exists(home + "aaaaabcc/") == false);

        FileManager::CreateFile(home + "abcdirexists/hiddenfile");
        FileManager::CreateDir(home + "abcdirexists/hiddendir/");
        QFile(home + "abcdirexists/").setPermissions(QFile::ExeOther);
        Q_ASSERT(FileManager::Exists(home + "abcdirexists/hiddenfile") == false);

    // IsFile

        // is file
        Q_ASSERT(FileManager::IsFile(home + "abcdexists"));
        Q_ASSERT(FileManager::IsFile(home + "abcdexists/") == false);

        // is not file
        Q_ASSERT(FileManager::IsFile(home + "abcdirexists") == false);
        Q_ASSERT(FileManager::IsFile(home + "abcdirexists/") == false);

        // is hidden
        Q_ASSERT(FileManager::IsFile(home + "abcdirexists/hiddenfile") == false);
        Q_ASSERT(FileManager::IsFile(home + "abcdirexists/hiddenfile/") == false);

    // IsDir

        // is dir
        Q_ASSERT(FileManager::IsDir(home + "abcdirexists"));
        Q_ASSERT(FileManager::IsDir(home + "abcdirexists/"));

        // is not dir
        Q_ASSERT(FileManager::IsDir(home + "abcdexists") == false);
        Q_ASSERT(FileManager::IsDir(home + "abcdexists/") == false);

        // is hidden
        Q_ASSERT(FileManager::IsDir(home + "abcdirexists/hiddendir") == false);
        Q_ASSERT(FileManager::IsDir(home + "abcdirexists/hiddendir/") == false);

    // GetDirContents - I "know it works"

    // RenameFile
        FileManager::CreateFile(home + "hellofile");
        FileManager::RenameFile(home + "hellofile", home + "hello2file");
        Q_ASSERT(!QFile(home+"hellofile").exists() && QFile(home + "hello2file").exists());
        Q_ASSERT(!FileManager::Exists(home+"hellofile") && FileManager::Exists(home + "hello2file"));
        Q_ASSERT(FileManager::IsFile(home +"hello2file"));
        Q_ASSERT(!FileManager::IsFile(home+"hellofile"));
        FileManager::DeleteFile(home + "hello2file");
        Q_ASSERT(!FileManager::Exists(home + "hello2file"));

        try
        {
            FileManager::RenameFile("/bin/bash", "/bin/hi");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException&)
        {
            Q_ASSERT(true);
        }
        try
        {
            FileManager::RenameFile(home + "doesntexist", "");
            Q_ASSERT(false);
        }
        catch (NotFoundException&)
        {
            Q_ASSERT(true);
        }
        FileManager::CreateFile(home+"hellofile");
        FileManager::CreateFile(home + "exists");
        try
        {
            FileManager::RenameFile(home + "exists", home + "hellofile");
            Q_ASSERT(false);
        }
        catch (AlreadyExistsException&)
        {
            Q_ASSERT(true);
        }
        FileManager::DeleteFile(home+"hellofile");
        FileManager::DeleteFile(home+"exists");

    // RenameDir
        FileManager::CreateDir(home + "helloworlddir");
        FileManager::RenameDir(home + "helloworlddir", home + "helloworld2dir");
        Q_ASSERT(!QDir(home+"helloworlddir").exists() && QDir(home + "helloworld2dir").exists());
        Q_ASSERT(!FileManager::Exists(home+"helloworlddir") && FileManager::Exists(home + "helloworld2dir"));
        Q_ASSERT(FileManager::IsDir(home +"helloworld2dir"));
        Q_ASSERT(!FileManager::IsDir(home+"helloworlddir"));
        FileManager::DeleteDir(home + "helloworld2dir");
        Q_ASSERT(!FileManager::Exists(home + "helloworld2dir"));

        try
        {
            FileManager::RenameDir("/etc/gdb", "/bin/hello");
            Q_ASSERT(false);
        }
        catch (ImproperPermissionsException&)
        {
            Q_ASSERT(true);
        }
        try
        {
            FileManager::RenameDir(home + "doesntexist", "");
            Q_ASSERT(false);
        }
        catch (NotFoundException&)
        {
            Q_ASSERT(true);
        }
        FileManager::CreateDir(home+"helloworlddir");
        FileManager::CreateDir(home + "existsdir");
        try
        {
            FileManager::RenameDir(home + "existsdir", home + "helloworlddir");
            Q_ASSERT(false);
        }
        catch (AlreadyExistsException&)
        {
            Q_ASSERT(true);
        }
        FileManager::DeleteDir(home+"helloworlddir");
        FileManager::DeleteDir(home+"existsdir");

}

