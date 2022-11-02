#include <QCoreApplication>
#include "krypto.h"
#include <QRandomGenerator>
#include <iostream>
#include <QFile>
#include <fstream>
#include <string>

void shamir_ce_file(QVector<qint64> &message){
    int p = 0;
    QVector<qint64> cada(2, 0), cbdb(2,0);
    for(qint64 i = QRandomGenerator::global()->bounded(10000000, 200000000); i > 0; i-- )
        if(is_prime_number(i)){
            p = i;
            break;
        }
    gen_CxDx(cada, p);
    gen_CxDx(cbdb, p);
    for(int i = 0; i < message.size(); i++){
        message[i] = shamir_coding(message[i], cada[0], p);
        message[i] = shamir_decoding(message[i], cbdb[0], cada[1], cbdb[1], p);
    }
}

void eg_ce_file(QVector<qint64> &message){
    qint64 i = QRandomGenerator::global()->bounded(1000000, 100000000);
    qint64 p = 0, q = 0, g = 0, ca = 0, da = 0, cb = 0, db = 0;
    for(; i > 100; i-- )
        if(is_prime_number(i)){
            if(is_prime_number((i*2)+1)){
                q = i;
                p = (q * 2)+1;
                break;
            }
        }
    do{
        g = QRandomGenerator::global()->bounded(2, p-2);
    }while(exp_mod(g,q,p)==1);
    ca = QRandomGenerator::global()->bounded(1, p-2);
    da = exp_mod(g, ca, p);
    cb = QRandomGenerator::global()->bounded(2, p-2);
    db = exp_mod(g, cb, p);
    for(int i = 0; i < message.size(); i++){
        message[i] = (message[i]%p * (exp_mod(db, ca, p)))%p;
        message[i] = (message[i]%p * (exp_mod(da, p-1-cb,p)))%p;
    }
}

void rsa_ce_file(QVector<qint64> &message){
    qint64 pb, qb, nb, fi;
    QVector<qint64> cbdb(2,0);
    pb = gen_prime_num(1000, 10000);
    qb = gen_prime_num(1000, 10000);
    nb = pb * qb;
    qDebug() << pb << " " << qb;
    fi = (pb-1) * (qb - 1);
    gen_CxDx(cbdb, fi+1);
    qDebug() << fi;
    qDebug() << cbdb;
    for(int i = 0; i < message.size(); i++){
        message[i] = exp_mod(message[i], cbdb[0],nb);
        qDebug() << "message = " << message[i];
        message[i] = exp_mod(message[i], cbdb[1], nb);
    }
}

QString to_binary_str(qint64 num){
    QString bin_str;
    while(num > 0){
        if(num % 2 == 1){
            bin_str+="1";
        }else{
            bin_str+="0";
        }
        num = num >> 1;
    }
    for(int i = 0; i < 20-bin_str.size(); i++){
        bin_str+="0";
    }
    char temp;
    for(int i = 0; i < bin_str.size()/2; i++){
        temp = bin_str[i].unicode();
        bin_str[i] = bin_str[bin_str.size()-1-i];
        bin_str[bin_str.size()-1-i] = temp;
    }
    return bin_str;
}

void vernam_ce_file(QVector<qint64> &message){
    QString e_str, k;
    QString m_str;
    for(int m_i = 0; m_i < message.size(); m_i++){
        k.clear();
        e_str.clear();
        m_str.clear();
        m_str = to_binary_str(message[m_i]);
        for(int i = 0; i < m_str.size(); i++){
            if(QRandomGenerator::global()->bounded(-1, 2)){
                e_str += "1";
                if(m_str[i] == "1"){
                    k += "0";
                }else{
                    k += "1";
                }
            }else{
                e_str += "0";
                if(m_str[i] == "0"){
                    k += "0";
                }else{
                    k += "1";
                }
            }
        }
        //        qDebug() << e_str;
        //        qDebug() << k;
        //        qDebug() << m_str;
        QString decod_msg;
        for(int i = 0; i < m_str.size(); i++){
            if(e_str[i] == k[i]){
                decod_msg += "0";
            }else{
                decod_msg += "1";
            }
        }
        qDebug() << decod_msg;
        //decode
        message[m_i] = 0;
        for(int i = decod_msg.size()-1, j = 0; i >= 0; i--, j++){
            if(decod_msg[i] == "1"){
                message[m_i] += qPow(2,j);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    enum{shamir = 1, eg = 2, rsa = 3, vernam = 4};
    QByteArray buf, buf1, buf2;
    QVector<qint64> buf1_toint;
    QFile in, out;

    std::string path_to_file;
    while(1){
        std::cin >> path_to_file;
        in.setFileName(path_to_file.c_str());
        if(in.open(QIODevice::ReadOnly)){
            break;
        }
        qDebug() << "Такого файла не существует!";
    }
    QString out_file_name;
    out_file_name = path_to_file.c_str();
    QStringList split_out_file_name = out_file_name.split(".");
    out.setFileName("out." + split_out_file_name[split_out_file_name.size()-1]);
    if(out.open(QIODevice::WriteOnly)){
        buf = in.readAll();
        qDebug() <<  buf;
        buf1 = buf.toBase64();
        qDebug() <<  buf1;
        for(int i = 0; i < buf1.size(); i++){
            buf1_toint << buf1[i];
        }
        int key;
        std::cin >> key;
        switch (key) {
        case shamir:
            shamir_ce_file(buf1_toint);
            break;
        case eg:
            eg_ce_file(buf1_toint);
            break;
        case rsa:
            rsa_ce_file(buf1_toint);
            break;
        case vernam:
            vernam_ce_file(buf1_toint);
            break;
        default:
            break;
        }
        buf1.clear();
        char test;
        for(int i = 0; i < buf1_toint.size(); i++){
            test = buf1_toint[i];
            buf1.append(test);
        }
        qDebug() << buf1_toint;
        buf2 = QByteArray::fromBase64(buf1);
        qDebug() << buf2;
        out.write(buf2);
    }
    out.close();
    in.close();
    return a.exec();
}
