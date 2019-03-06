#include "postrequesthandler.h"

PostRequestHandler::PostRequestHandler(QSqlDatabase *db, Request *request):DB_(db),Request_(request)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

QString PostRequestHandler::registrHandler(QString post)
{

    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();
        //QJsonObject user = obj["Data"].toObject();

        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `auth`.`login` FROM `auth` WHERE `auth`.`login` = \""+obj["login"].toString()+"\"");
        query->exec();

        if(query->size()==0){
            query->prepare("INSERT INTO `auth`(`login`, `password`, `id_role`) VALUES ('"+obj["login"].toString()+"','"+obj["password"].toString()+"',(SELECT `role`.`id_role` FROM `role` WHERE `role`.`role`= \""+obj["role"].toString()+"\"))");
            query->exec();
            return QString ("YES");
        }
        else{
            return QString("NO");
        }
    }
    else return QString("NO");
}

QString PostRequestHandler::newIngredientHandler(QString post)
{
    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();

        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `ingredients`.`title_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient` =\""+obj["title"].toString()+"\"");
        query->exec();
        if(query->size()==0){
            int id = 1;
            bool f =1;
            while (f) {
                query->prepare("SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `id_ingredient` = " + QString::number(id));
                query->exec();
                if(query->size()==0){
                    f=0;
                }
                else {
                    id++;
                }
            }

            query->prepare("INSERT INTO `ingredients`(`id_ingredient`, `title_ingredient`, `unit`) VALUES ('"+QString::number(id)+"','"+obj["title"].toString()+"','"+obj["unit"].toString()+"')");
            query->exec();
            return QString("YES");
        }
        else {
            return QString("Ingredients exist");
        }
    }
    else return QString("NO");
}

QString PostRequestHandler::newStockHandler(QString post)
{
    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();

        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `stock`.`title_stock` FROM `stock` WHERE `stock`.`title_stock` =\""+obj["title"].toString()+"\"");
        query->exec();
        if(query->size()==0){
            int id = 1;
            bool f =1;
            while (f) {
                query->prepare("SELECT `stock`.`id_stock` FROM `stock` WHERE `stock`.`id_stock` = " + QString::number(id));
                query->exec();
                if(query->size()==0){
                    f=0;
                }
                else {
                    id++;
                }
            }

            query->prepare("INSERT INTO `stock`(`id_stock`, `title_stock`) VALUES ('"+QString::number(id)+"','"+obj["title"].toString()+"')");
            query->exec();
            return QString("YES");
        }
        else {
            return QString("Stock exist");
        }
    }
    else return QString("NO");
}

QString PostRequestHandler::newDishHandler(QString post)
{
    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();

        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `dish`.`title_dish` FROM `dish` WHERE `dish`.`title_dish` =\""+obj["title"].toString()+"\"");
        query->exec();
        if(query->size()==0){
            int id = 1;
            bool f =1;
            while (f) {
                query->prepare("SELECT `dish`.`id_dish` FROM `dish` WHERE `dish`.`id_dish` = " + QString::number(id));
                query->exec();
                if(query->size()==0){
                    f=0;
                }
                else {
                    id++;
                }
            }

            query->prepare("INSERT INTO `dish`(`id_dish`, `title_dish`) VALUES ('"+QString::number(id)+"','"+obj["title"].toString()+"')");
            query->exec();

            QJsonArray arr = obj["ingredients"].toArray();
            for (int i=0; i<arr.size();i++){
                query->prepare("INSERT INTO `ingredients-dish`(`id_dish`, `id_ingredient`, `amount_ingredient`) VALUES ('"+QString::number(id)+"',(SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient` = \""+arr[i].toObject().value("title").toString()+"\" ),'"+arr[i].toObject().value("amount").toString()+"')");
                query->exec();
            }


            return QString("YES");
        }
        else {
            return QString("Dish exist");
        }
    }
    else return QString("NO");
}

QString PostRequestHandler::newGuestHandler(QString post)
{
    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();

        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `guests`.`pasport` FROM `guests` WHERE `guests`.`pasport` =\""+obj["pasport"].toString()+"\"");
        query->exec();
        if(query->size()==0){
            int id = 1;
            bool f =1;
            while (f) {
                query->prepare("SELECT `guests`.`id_guest` FROM `guests` WHERE `guests`.`id_guest` = " + QString::number(id));
                query->exec();
                if(query->size()==0){
                    f=0;
                }
                else {
                    id++;
                }
            }


            QString tamplate = "INSERT INTO `guests`(`id_guest`, `pasport`, `second_name`, `first_name`, `patronymic`, `telephone`, `settlement_date`, `eviction_date`) VALUES ('%1','%2','%3','%4','%5','%6','%7','%8')";
            query->prepare(tamplate.arg(QString::number(id)).arg(obj["pasport"].toString()).arg(obj["second_name"].toString()).arg(obj["first_name"].toString()).arg(obj["patronymic"].toString()).arg(obj["telephone"].toString()).arg(obj["settlement_date"].toString()).arg(obj["eviction_date"].toString()));
            query->exec();



            return QString("YES");
        }
        else {
            return QString("Guest exist");
        }
    }
    else return QString("NO");
}

QString PostRequestHandler::updateGuestHandler(QString post)
{
    if(post != ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();
        QSqlQuery *query = new QSqlQuery(*DB_);
        query->prepare("SELECT `guests`.`pasport` FROM `guests` WHERE `guests`.`pasport` =\""+obj["pasport"].toString()+"\"");
        query->exec();
        if(query->size()!=0){
            QString tamplate ="UPDATE `guests` SET`pasport`='%1',`second_name`='%2',`first_name`='%3',`patronymic`='%4',`telephone`='%5',`settlement_date`='%6',`eviction_date`='%7' WHERE `guests`.`pasport` = \"%8\"";
            query->prepare(tamplate.arg(obj["pasport"].toString()).arg(obj["second_name"].toString()).arg(obj["first_name"].toString()).arg(obj["patronymic"].toString()).arg(obj["telephone"].toString()).arg(obj["settlement_date"].toString()).arg(obj["eviction_date"].toString()).arg(obj["id"].toString()));
            query->exec();
            return QString("YES");
        }
        else {
            return QString("Pasport not found");
        }

    }
    else return QString("NO");
}

QString PostRequestHandler::addMenuHandler(QString post)
{
    //{"date":"2019-02-10","amount":"30","type":"dinner","dishs":["dish1","dish2"]}

    if(post!= ""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();
        QSqlQuery *query = new QSqlQuery(*DB_);
        int id = 1;
        bool f =1;
        while (f) {
            query->prepare("SELECT `menu`.`id_menu` FROM `menu` WHERE `menu`.`id_menu` = " + QString::number(id));
            query->exec();
            if(query->size()==0){
                f=0;
            }
            else {
                id++;
            }
        }
        query->prepare("INSERT INTO `menu`(`id_menu`, `date_menu`, `id_type`, `amount_portion`) VALUES ('"+QString::number(id)+"','"+obj["date"].toString()+"',(SELECT `type_menu`.`id_type` FROM `type_menu` WHERE `type_menu`.`title_type` = \""+obj["type"].toString()+"\" ),"+QString::number(obj["amount"].toInt())+")");
        query->exec();
        QJsonArray dishs = doc["dishs"].toArray();

        for(int i=0; i< dishs.size();i++){
            query->prepare("INSERT INTO `menu-dish`(`id_menu`, `id_dish`) VALUES ('"+QString::number(id)+"',(SELECT `dish`.`id_dish` FROM `dish` WHERE `dish`.`title_dish` = \""+dishs[i].toString()+"\"))");
            query->exec();

            query->prepare("SELECT `ingredients`.`title_ingredient` , `ingredients-dish`.`amount_ingredient` FROM `ingredients-dish` INNER JOIN `ingredients` ON `ingredients-dish`.`id_ingredient` = `ingredients`.`id_ingredient` INNER JOIN `dish` ON `ingredients-dish`.`id_dish` = `dish`.`id_dish` WHERE `dish`.`title_dish` = \""+dishs[i].toString()+"\"");
            query->exec();
            while (query->next()) {
                int count = query->value(1).toInt()*obj["amount"].toInt();
                QSqlQuery* qry = new QSqlQuery(*DB_);
                qry->prepare("SELECT `ingredients-stock`.`id_stock`, `ingredients`.`title_ingredient`, `ingredients-stock`.`amount_ingredient` FROM `ingredients-stock` INNER JOIN `ingredients` ON `ingredients-stock`.`id_ingredient` = `ingredients`.`id_ingredient` WHERE `ingredients`.`title_ingredient` = \""+query->value(0).toString()+"\"");
                qry->exec();
                while(qry->next()){
                    while (count!=0) {
                        int ing_am = qry->value(2).toInt();
                        if(count >= ing_am){
                            QSqlQuery *upd = new QSqlQuery(*DB_);
                            upd->prepare("UPDATE `ingredients-stock` SET `amount_ingredient`= 0  WHERE `ingredients-stock`.`id_stock` = "+qry->value(0).toString());
                            upd->exec();
                            count -= ing_am;
                        }
                        else {
                            int sub = ing_am - count;
                            QSqlQuery *upd = new QSqlQuery(*DB_);
                            upd->prepare("UPDATE `ingredients-stock` SET `amount_ingredient`= "+QString::number(sub)+"  WHERE `ingredients-stock`.`id_stock` = "+qry->value(0).toString());
                            upd->exec();
                            count = 0;
                        }
                    }
                }
            }

        }


        return QString ("YES");
    }
    else {
        return QString ("NO");
    }

}

QString PostRequestHandler::addInvoiceHandler(QString post)
{
    //{"date":"2019-02-13","stock":"Подвал","id":"982", "ingredients": [{"title":"Морковь","amount":20},...]}

    if(post!=""){
        QJsonDocument doc = QJsonDocument::fromJson(post.toLocal8Bit());
        QJsonObject obj = doc.object();
        QSqlQuery *query = new QSqlQuery(*DB_);
        QString stock = obj["stock"].toString();
        query->prepare("SELECT `invoice`.`id_invoice` FROM `invoice` WHERE `invoice`.`id_invoice` = "+obj["id"].toString());
        query->exec();
        if(query->size()==0){

            query->prepare("INSERT INTO `invoice`(`id_invoice`, `date_invoice`) VALUES ('"+obj["id"].toString()+"','"+obj["date"].toString()+"')");
            query->exec();

            QJsonArray ingredients = obj["ingredients"].toArray();
            for (int i=0;i<ingredients.size();i++) {
                QString ing_title = ingredients[i].toObject().value("title").toString();
                int amount = ingredients[i].toObject().value("amount").toInt();
                query->prepare("INSERT INTO `invoice-stock`(`id_invoice`, `id_ingredient`, `amount_ingredient`, `id_stock`) VALUES ('"+obj["id"].toString()+"',(SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient` = \""+ing_title+"\" ),'"+QString::number(amount)+"',(SELECT `stock`.`id_stock` FROM `stock` WHERE `stock`.`title_stock`= \""+stock+"\"))");
                query->exec();

                QSqlQuery* qry = new QSqlQuery(*DB_);
                qry->prepare("SELECT * FROM `ingredients-stock` WHERE `ingredients-stock`.`id_ingredient` = (SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient`= \""+ing_title+"\" ) AND `ingredients-stock`.`id_stock` = (SELECT `stock`.`id_stock` FROM `stock` WHERE `stock`.`title_stock` = \""+stock+"\")");
                qry->exec();
                if(qry->size()==0){
                    qry->prepare("INSERT INTO `ingredients-stock`(`id_stock`, `id_ingredient`, `amount_ingredient`) VALUES ((SELECT `stock`.`id_stock` FROM `stock` WHERE `stock`.`title_stock` = \""+stock+"\"),(SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient`= \""+ing_title+"\"),'"+QString::number(amount)+"')");
                    qry->exec();
                }
                else {
                    while (qry->next()) {
                        QString id_ing = qry->record().value(1).toString();
                        QString id_stock = qry->record().value(0).toString();
                        int count = qry->value(2).toInt() + amount;
                        qDebug() << id_ing << " " << id_stock << " " << count;
                        qry->prepare("UPDATE `ingredients-stock` SET `amount_ingredient` = "+QString::number(count)+" WHERE (`ingredients-stock`.`id_stock` = \""+id_stock+"\")  AND (`ingredients-stock`.`id_ingredient` = \""+id_ing+"\") ");
                        qry->exec();
                    }

                }
            }
            return QString("YES");
        }
        else {
            return QString("Invoice exsist");
        }
    }
    else {
        return QString("NO");
    }


}
