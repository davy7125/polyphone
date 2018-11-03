#include "outputsf3.h"
#include "soundfontmanager.h"
#include "sfont.h"

OutputSf3::OutputSf3() : AbstractOutput() {}

void OutputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
//    // Temporary file
//    QString fileNameSf2 = fileName.left(fileName.length() - 4) + "_tmp.sf2";
//    if (QFile(fileNameSf2).exists()) {
//        int index = 2;
//        QString left = fileNameSf2.left(fileNameSf2.length() - 8);
//        while (QFile(left + "-" + QString::number(index) + "_tmp.sf2").exists())
//            index++;
//        fileNameSf2 = left + "-" + QString::number(index) + "_tmp.sf2";
//    }

//    // Save sf2 first
//    if (this->sauvegarderSf2(indexSf2, fileNameSf2) == 0)
//    {
//        // Then create sf3
//        SfConvert::SoundFont sf(fileNameSf2);
//        if (sf.read())
//        {
//            QFile fo(fileName);
//            if (fo.open(QIODevice::WriteOnly))
//            {
//                sf.write(&fo, quality);
//                fo.close();
//            }
//            else
//                ret = 3;
//        }
//        else
//            ret = 3;
//    }
//    else
//        ret = 3;

//    // Delete the sf2 temporary file
//    QFile::remove(fileNameSf2);
}
