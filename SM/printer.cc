#include "printer.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <glog/logging.h>

using namespace std;

//输出一些空格，以便打印条目将所有内容对齐
void Spaces(int maxLength, size_t printedSoFar) {
    for (int i = printedSoFar; i < maxLength; i++)
        cout << " ";
}

//构造函数，元组的打印
Printer::Printer(const std::vector<DataAttrInfo> &attributes_) {
    attrCount = attributes_.size();
    attributes = new DataAttrInfo[attrCount];

    for (int i = 0; i < attrCount; i++)
        attributes[i] = attributes_[i];

    //打印的元组数
    iCount = 0;

    //标题信息
    psHeader = (char**)malloc(attrCount * sizeof(char*));

    //每个属性之间的空格数
    spaces = new int[attrCount];

    //需要打印的属性数
    for (int i = 0; i < attrCount; i++ ) {
        //尝试在另一列中查找属性
        int bFound = 0;
        psHeader[i] = new char[MAXPRINTSTRING];
        memset(psHeader[i], 0, MAXPRINTSTRING);

        for (int j = 0; j < attrCount; j++)
            if (j != i &&
                    strcmp(attributes[i].attrName,
                           attributes[j].attrName) == 0) {
                bFound = 1;
                break;
            }

        if (bFound)
            sprintf(psHeader[i], "%s.%s",
                    attributes[i].relName, attributes[i].attrName);
        else
            strcpy(psHeader[i], attributes[i].attrName);

        if (attributes[i].attrType == STRING)
            spaces[i] = min(attributes[i].attrDisplayLength, MAXPRINTSTRING);
        else
            spaces[i] = max(12, (int)strlen(psHeader[i]));

        //减去标头
        spaces[i] -= strlen(psHeader[i]);

        //如果有负数（或零）空格，则插入一个
        if (spaces[i] < 1) {
            spaces[i] = 0;
            strcat(psHeader[i], " ");
        }
    }
}


//析构函数
Printer::~Printer() {
    for (int i = 0; i < attrCount; i++)
        delete [] psHeader[i];

    delete [] spaces;
    //删除psHeader
    free (psHeader);
    delete [] attributes;
}


//打印header
void Printer::PrintHeader( ostream &c ) const {
    int dashes = 0;
    int iLen;
    int i, j;

    for (i = 0; i < attrCount; i++) {
        //打印标题信息名称
        c << psHeader[i];
        iLen = (int)strlen(psHeader[i]);
        dashes += iLen;

        for (j = 0; j < spaces[i]; j++)
            c << " ";

        dashes += spaces[i];
    }

    c << "\n";
    for (i = 0; i < dashes; i++) c << "-";
    c << "\n";
}

void Printer::myPrintHeader(ostream& c, char *selheader,char * cluheader) {
    int dashes = 0;
    int iLen;
    int i, j;

    //打印标题信息名称
    c << selheader;
    iLen = (int)strlen(selheader);
    dashes += iLen;
    for (j = 0; j < 5; j++)
        c << " ";
    dashes += 5;

    c << cluheader;
    iLen = (int)strlen(selheader);
    dashes += iLen;
    for (j = 0; j < 5; j++)
        c << " ";
    dashes += 5;

    c << "\n";
    for (i = 0; i < dashes; i++) c << "-";
    c << "\n";
}

void Printer::myPrintHeader2(ostream &c, int attrCount, DataAttrInfo *attributes) {
    int dashes = 0;
    int iLen;
    int i, j;

    //打印标题信息名称
    for (int num=0;num<attrCount;num++){
        c << attributes[num].attrName;
        iLen = (int)strlen(attributes[num].attrName);
        dashes += iLen;
        for (j = 0; j < 5; j++)
            c << " ";
        dashes += 5;
    }

    c << "\n";
    for (i = 0; i < dashes; i++) c << "-";
    c << "\n";
}

//打印footer
void Printer::PrintFooter(ostream &c) const {
    c << "\n";
    c << iCount << " tuple(s).\n";
}


//打印函数
void Printer::Print(ostream &c, const char * const data, bool isnull[]) {
    char str[MAXPRINTSTRING], strSpace[50];
    int i, a;
    float b;

    if (data == NULL)
        return;

    //增加打印的元组数
    iCount++;

    int nullableIndex = 0;

    for (i = 0; i < attrCount; i++) {
        bool this_isnull = false;
        if (!(attributes[i].attrSpecs & ATTR_SPEC_NOTNULL)) {
            this_isnull = isnull[nullableIndex++];
        }
        if (attributes[i].attrType == STRING || this_isnull) {
            //打印出前MAXNAME + 10个字符
            memset(str, 0, MAXPRINTSTRING);

            const char* str_to_print = this_isnull ? "NULL" : data + attributes[i].offset;

            if (attributes[i].attrDisplayLength > MAXPRINTSTRING) {
                strncpy(str, str_to_print, MAXPRINTSTRING - 1);
                str[MAXPRINTSTRING - 3] = '.';
                str[MAXPRINTSTRING - 2] = '.';
                c << str;
                Spaces(MAXPRINTSTRING, strlen(str));
            } else {
                strncpy(str, str_to_print, (size_t)(this_isnull ? 4 : attributes[i].attrDisplayLength));
                c << str;
                if (attributes[i].attrDisplayLength < (int) strlen(psHeader[i]))
                    Spaces((int)strlen(psHeader[i]), strlen(str));
                else
                    Spaces(attributes[i].attrDisplayLength, strlen(str));
            }
        } else if (attributes[i].attrType == INT) {
            memcpy (&a, (data + attributes[i].offset), sizeof(int));
            sprintf(strSpace, "%d", a);
            c << a;
            if (strlen(psHeader[i]) < 12)
                Spaces(12, strlen(strSpace));
            else
                Spaces((int)strlen(psHeader[i]), strlen(strSpace));
        } else if (attributes[i].attrType == FLOAT) {
            memcpy (&b, (data + attributes[i].offset), sizeof(float));
            sprintf(strSpace, "%f", b);
            c << strSpace;
            if (strlen(psHeader[i]) < 12)
                Spaces(12, strlen(strSpace));
            else
                Spaces((int)strlen(psHeader[i]), strlen(strSpace));
        }
    }
    c << "\n";
}

void Printer::myPrint(ostream & c, const int *seldata, int sellen, int selheadlen, const int *cludata, int clulen, int cluheadlen) {
    char str[MAXPRINTSTRING], strSpace[50];
    int a;
    int b;
    memset(str, 0, MAXPRINTSTRING);
    memcpy(&a, seldata, size_t(sellen));
    c << str;
        if (sellen < selheadlen)
            Spaces(selheadlen, strlen(str));
        else
            Spaces(sellen, strlen(str));

    memcpy(&b, cludata, sizeof(int));
    sprintf(strSpace, "%d", b);
    c << strSpace;
    if (cluheadlen< 12)
        Spaces(12, strlen(strSpace));
    else
        Spaces(cluheadlen, strlen(strSpace));

    c << "\n";
}

void Printer::myPrint2(ostream & c, const char *seldata, int sellen, int selheadlen, const int *cludata, int clulen, int cluheadlen) {
    char str[MAXPRINTSTRING], strSpace[50];
    int a;
    int b;
    memset(str, 0, MAXPRINTSTRING);
    memcpy(str, seldata, sizeof(int));
    sprintf(str, "%d", a);
    c << str;
    if (sellen < selheadlen)
        Spaces(selheadlen, strlen(str));
    else
        Spaces(sellen, strlen(str));

    memcpy(&b, cludata, sizeof(int));
    sprintf(strSpace, "%d", b);
    c << strSpace;
    if (cluheadlen< 12)
        Spaces(12, strlen(strSpace));
    else
        Spaces(cluheadlen, strlen(strSpace));

    c << "\n";
}



void Printer::myPrinter(std::ostream &c,const char * const data, DataAttrInfo *attributes,int attrCount, bool isnull[]) {
//    char str1[MAXPRINTSTRING],str2[MAXPRINTSTRING] ;
//
//    if (sellen1 > MAXPRINTSTRING) {
//        strncpy(str1, seldata1, MAXPRINTSTRING - 1);
//        str1[MAXPRINTSTRING - 3] = '.';
//        str1[MAXPRINTSTRING - 2] = '.';
//        c << str1;
//        Spaces(MAXPRINTSTRING, strlen(str1));
//    } else {
//        strncpy(str1, seldata1, (size_t)(sellen1));
//        c << str1;
//        if (sellen1< selheadlen1)
//            Spaces(selheadlen1, strlen(str1));
//        else
//            Spaces(sellen1, strlen(str1));
//    }
//
//    if (sellen2 > MAXPRINTSTRING) {
//        strncpy(str2, seldata2, MAXPRINTSTRING - 1);
//        str1[MAXPRINTSTRING - 3] = '.';
//        str1[MAXPRINTSTRING - 2] = '.';
//        c << str2;
//        Spaces(MAXPRINTSTRING, strlen(str2));
//    } else {
//        strncpy(str2, seldata2, (size_t)(sellen2));
//        c << str2;
//        if (sellen2< selheadlen2)
//            Spaces(selheadlen2, strlen(str2));
//        else
//            Spaces(sellen2, strlen(str2));
//    }
//    c << "\n";
    char str[MAXPRINTSTRING], strSpace[50];
    int i, a;
    float b;

    if (data == NULL)
        return;
    int nullableIndex = 0;
    for (i = 0; i < attrCount; i++) {
        bool this_isnull = false;
        if (!(attributes[i].attrSpecs & ATTR_SPEC_NOTNULL)) {
            this_isnull = isnull[nullableIndex++];
        }
        if (attributes[i].attrType == STRING || this_isnull) {
            //打印出前MAXNAME + 10个字符
            memset(str, 0, MAXPRINTSTRING);

            const char* str_to_print = this_isnull ? "NULL" : data + attributes[i].offset;

            if (attributes[i].attrDisplayLength > MAXPRINTSTRING) {
                strncpy(str, str_to_print, MAXPRINTSTRING - 1);
                str[MAXPRINTSTRING - 3] = '.';
                str[MAXPRINTSTRING - 2] = '.';
                c << str;
                Spaces(MAXPRINTSTRING, strlen(str));
            } else {
                strncpy(str, str_to_print, (size_t)(this_isnull ? 4 : attributes[i].attrDisplayLength));
                c << str;
                if (attributes[i].attrDisplayLength < (int) strlen(attributes[i].attrName))
                    Spaces((int)strlen(attributes[i].attrName), strlen(str));
                else
                    Spaces(attributes[i].attrDisplayLength, strlen(str));
            }
        } else if (attributes[i].attrType == INT) {
            memcpy (&a, (data + attributes[i].offset), sizeof(int));
            sprintf(strSpace, "%d", a);
            c << a;
            if (strlen(attributes[i].attrName) < 12)
                Spaces(12, strlen(strSpace));
            else
                Spaces((int)strlen(attributes[i].attrName), strlen(strSpace));
        } else if (attributes[i].attrType == FLOAT) {
            memcpy (&b, (data + attributes[i].offset), sizeof(float));
            sprintf(strSpace, "%f", b);
            c << strSpace;
            if (strlen(attributes[i].attrName) < 12)
                Spaces(12, strlen(strSpace));
            else
                Spaces((int)strlen(attributes[i].attrName), strlen(strSpace));
        }
    }
    c << "\n";
}