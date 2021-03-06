#include "getrequesthandler.h"

GetRequestHandler::GetRequestHandler(QSqlDatabase *db, Request *request):DB_(db),Request_(request)
{
QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

QString GetRequestHandler::dishHandle(){


    QJsonArray dishs_array;
    QSqlQuery* q = new QSqlQuery(*DB_);
    if (q->exec("SELECT * FROM dish")) {
        while (q->next()) {
            QJsonObject dish_obj;
            dish_obj["id"] = q->value(0).toString();
            dish_obj["dish"] = q->value(1).toString();
            QJsonArray ingredients;

            QSqlQuery* qry = new QSqlQuery(*DB_);
            qry->exec("SELECT `ingredients`.`title_ingredient`, `ingredients_dish`.`amount_ingredient`, `ingredients`.`unit` FROM `ingredients` INNER JOIN (`dish` INNER JOIN `ingredients_dish` ON `dish`.`id_dish` = `ingredients_dish`.`id_dish`) ON `ingredients`.`id_ingredient` = `ingredients_dish`.`id_ingredient`WHERE (((`dish`.`title_dish`)=\""+dish_obj["dish"].toString()+"\"))");

            while (qry->next()) {
                QJsonObject ingr_obj;
                ingr_obj["title"]=qry->value(0).toString();
                ingr_obj["amount"]=qry->value(1).toString();
                ingredients.append(ingr_obj);


            }
            delete qry;
            dish_obj["ingredients"]=ingredients;
            dishs_array.append(dish_obj);
        }
    }

    QJsonObject dish;
    dish["Dishs"]=dishs_array;
    QJsonArray ingredients;
    if (q->exec("SELECT * FROM ingredients"))
        while (q->next()) {
            QJsonObject ingredient;
            ingredient["title"]=q->value(1).toString();
            ingredient["unit"]=q->value(3).toString();
            ingredient["needonstock"]=q->value(2).toString();

            QSqlQuery* q = new QSqlQuery(*DB_);
            q->exec("SELECT SUM(`ingredients_stock`.`amount_ingredient`) FROM `ingredients_stock` WHERE `ingredients_stock`.`id_ingredient` = (SELECT `ingredients`.`id_ingredient` FROM `ingredients` WHERE `ingredients`.`title_ingredient` = \""+ingredient["title"].toString()+"\")");
            while (q->next()) {
                ingredient["total_amount"]=q->value(0).toString();
            }
            ingredients.append(ingredient);
        }
    dish["Ingredients"]=ingredients;
    QJsonDocument doc;
    doc.setObject(dish);
    delete q;
    return QString(doc.toJson());

}

QString GetRequestHandler::StockIngHandler(){

    QJsonArray stock_ing_array;
    QSqlQuery* query = new QSqlQuery(*DB_);
    if (query->exec("SELECT `stock`.`title_stock`, `ingredients`.`title_ingredient`, `ingredients_stock`.`amount_ingredient`, `ingredients`.`unit` FROM `ingredients_stock` INNER JOIN `stock` ON `ingredients_stock`.`id_stock` = `stock`.`id_stock` INNER JOIN `ingredients` ON `ingredients_stock`.`id_ingredient`= `ingredients`.`id_ingredient`")) {
        while (query->next()) {
            QJsonObject stock_ing_obj;
            stock_ing_obj["stock_title"] = query->value(0).toString();
            stock_ing_obj["ingredient_title"] = query->value(1).toString();
            stock_ing_obj["amount"] = query->value(2).toString();
            stock_ing_obj["unit"] = query->value(3).toString();
            stock_ing_array.append(stock_ing_obj);
        }
    }
    QJsonObject stock;
    stock["ingredient_stock"]=stock_ing_array;

    QJsonArray stocks;
    if(query->exec("SELECT * FROM stock"))
        while (query->next()) {
            stocks.append(query->value(1).toString());
        }
    stock["Stocks"]=stocks;

    QJsonArray ingredients;
    if(query->exec("SELECT * FROM ingredients"))
        while (query->next()) {
            ingredients.append(query->value(1).toString());
        }
    stock["Ingredients"]=ingredients;

    QJsonDocument doc;
    doc.setObject(stock);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::menuHandler(){

    QJsonArray menu_arr;
    QSqlQuery* query = new QSqlQuery(*DB_);
    if(query->exec("SELECT `menu`.`id_menu`, `menu`.`date_menu`, `type_menu`.`title_type`, `menu`.`amount_portion` FROM `menu` INNER JOIN `type_menu` ON `type_menu`.`id_type`=`menu`.`id_type`"))
        while (query->next()) {
            QJsonObject menu_obj;
            menu_obj["id"]=query->value(0).toString();
            menu_obj["date"]=query->value(1).toString();
            menu_obj["type"]=query->value(2).toString();
            menu_obj["amount"]=query->value(3).toString();

            QSqlQuery* qry = new QSqlQuery(*DB_);
            QJsonArray dishs;
            if(qry->exec("SELECT `dish`.`title_dish` FROM `menu_dish` INNER JOIN `dish` ON `menu_dish`.`id_dish`=`dish`.`id_dish` WHERE `menu_dish`.`id_menu`= "+menu_obj["id"].toString()))
                while (qry->next()) {
                    dishs.append(qry->value(0).toString());
                }
            delete qry;
            menu_obj["dishs"]=dishs;
            menu_arr.append(menu_obj);
        }
    QJsonObject menu;
    menu["Menu"]=menu_arr;
    QJsonDocument doc;
    doc.setObject(menu);
    delete query;

    return QString(doc.toJson());
}

QString GetRequestHandler::authHandler()
{

    QJsonArray user_arr;
    QSqlQuery* query = new QSqlQuery(*DB_);
    if(query->exec("SELECT `auth`.`login`, `auth`.`password`, `role`.`role` FROM `auth` INNER JOIN `role` ON `auth`.`id_role`=`role`.`id_role`"))
        while (query->next()) {
            QJsonObject user_obj;
            user_obj["login"]=query->value(0).toString();
            user_obj["password"]=query->value(1).toString();
            user_obj["role"]=query->value(2).toString();
            user_arr.append(user_obj);
        }
    QJsonObject users;
    users["Users"]=user_arr;
    QJsonDocument doc;
    doc.setObject(users);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::invoiceHandler()
{

    QJsonArray inv_arr;
    QSqlQuery *query = new QSqlQuery(*DB_);
    if(query->exec("SELECT `invoice_stock`.`id_invoice`, `invoice`.`date_invoice`, `stock`.`title_stock`, `ingredients`.`title_ingredient`, `invoice_stock`.`amount_ingredient`, `ingredients`.`unit` FROM `invoice` INNER JOIN `invoice_stock` ON `invoice`.`id_invoice` = `invoice_stock`.`id_invoice` INNER JOIN `stock` ON `invoice_stock`.`id_stock`=`stock`.`id_stock` INNER JOIN `ingredients` ON `invoice_stock`.`id_ingredient`=`ingredients`.`id_ingredient`"))
        while (query->next()) {
            QJsonObject inv_obj;
            inv_obj["id"]=query->value(0).toString();
            inv_obj["date"]=query->value(1).toString();
            inv_obj["title-stock"]=query->value(2).toString();
            inv_obj["title-ingredient"]=query->value(3).toString();
            inv_obj["amount"]=query->value(4).toString();
            inv_obj["unit"]=query->value(5).toString();
            inv_arr.append(inv_obj);
        }
    QJsonObject invoice;
    invoice["Invoice"]=inv_arr;
    QJsonArray stocks;
    if(query->exec("SELECT * FROM stock"))
        while (query->next()) {
            stocks.append(query->value(1).toString());
        }
    invoice["Stocks"]=stocks;

    QJsonArray ingredients;
    if(query->exec("SELECT * FROM ingredients"))
        while (query->next()) {
            ingredients.append(query->value(1).toString());
        }
    invoice["Ingredients"]=ingredients;
    QJsonDocument doc;
    doc.setObject(invoice);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::wkInfoHandler()
{
    QJsonObject info;
    QSqlQuery *query = new QSqlQuery(*DB_);
    QJsonArray ingredients;
    query->exec("SELECT * FROM ingredients");
    while(query->next()){
        QJsonObject ingredient;
        ingredient["id"]=query->value(0).toString();
        ingredient["title"]=query->value(1).toString();
        ingredient["needstock"]=query->value(2).toString();
        ingredient["unit"]=query->value(3).toString();
        ingredients.append(ingredient);
    }
    info["Ingredients"]=ingredients;
    QJsonArray invoices;
    query->exec("SELECT * FROM invoice");
    while (query->next()) {
        QJsonObject invoice;
        invoice["id"]=query->value(0).toString();
        invoice["date"]=query->value(1).toString();
        invoices.append(invoice);
    }
    info["Invoices"]=invoices;
    QJsonArray stocks;
    query->exec("SELECT * FROM stock");
    while (query->next()) {
        stocks.append(query->value(1).toString());
    }
    info["Stocks"]=stocks;
    QJsonDocument doc;
    doc.setObject(info);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::guestsTableHandler()
{
    QJsonObject table;
    table["Name"] = "guests";
    QJsonArray guests;
    QSqlQuery* query= new QSqlQuery(*DB_);
    query->exec("SELECT * FROM guests");
    while (query->next()) {
        QJsonObject guest;
        guest["id"]=query->value(0).toString();
        guest["pasport"]=query->value(1).toString();
        guest["second_name"]=query->value(2).toString();
        guest["first_name"]=query->value(3).toString();
        guest["patronymic"]=query->value(4).toString();
        guest["telephone"]=query->value(5).toString();
        guest["settlement_date"]=query->value(6).toString();
        guest["eviction_date"]=query->value(7).toString();
        guests.append(guest);
    }
    table["Data"]=guests;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}


QString GetRequestHandler::guestCountHandler()
{
    QJsonObject count;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT COUNT(`guests`.`id_guest`) FROM `guests` WHERE `guests`.`settlement_date`<= '"+Request_->GetCgi("date")+"' AND `guests`.`eviction_date` >= '"+Request_->GetCgi("date")+"'");
    while (query->next()) {
        count["count"] = query->value(0).toString();
    }
    QJsonDocument doc;
    doc.setObject(count);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::authTableHandler()
{
    QJsonObject table;
    table["Name"] = "auth";
    QJsonArray auth;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `auth`");
    while (query->next()) {
        QJsonObject user;
        user["login"]=query->value(0).toString();
        user["password"]=query->value(1).toString();
        user["id_role"]=query->value(2).toString();
        auth.append(user);
    }
    table["Data"]=auth;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::dishTableHandler()
{
    QJsonObject table;
    table["Name"] = "dish";
    QJsonArray dishs;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `dish`");
    while (query->next()) {
        QJsonObject dish;
        dish["id"]=query->value(0).toString();
        dish["title"]=query->value(1).toString();
        dishs.append(dish);
    }
    table["Data"]=dishs;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::ingredientsTableHandler()
{
    QJsonObject table;
    table["Name"] = "ingredients";
    QJsonArray ingredients;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `ingredients`");
    while(query->next()){
        QJsonObject ingredient;
        ingredient["id"]=query->value(0).toString();
        ingredient["title"]=query->value(1).toString();
        ingredient["needonstock"]=query->value(2).toString();
        ingredient["unit"]=query->value(3).toString();
        ingredients.append(ingredient);
    }
    table["Data"]=ingredients;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::ingredients_dishTableHandler()
{
    QJsonObject tab;
    tab["Name"] = "ingredients_dish";
    QJsonArray table;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `ingredients_dish`");
    while(query->next()){
        QJsonObject row;
        row["id_dish"]=query->value(0).toString();
        row["id_ingredient"]=query->value(1).toString();
        row["amount"]=query->value(2).toString();
        table.append(row);
    }
    tab["Data"]=table;
    QJsonDocument doc;
    doc.setObject(tab);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::ingredients_stockHandler()
{
    QJsonObject tab;
    tab["Name"] = "ingredients_stock";
    QJsonArray table;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `ingredients_stock`");
    while (query->next()) {
        QJsonObject row;
        row["id_stock"]=query->value(0).toString();
        row["id_ingredient"]=query->value(1).toString();
        row["amount"]=query->value(2).toString();
        table.append(row);
    }
    tab["Data"]=table;
    QJsonDocument doc;
    doc.setObject(tab);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::invoiceTableHandler()
{
    QJsonObject table;
    table["Name"] = "invoice";
    QJsonArray invoices;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `invoice`");
    while (query->next()) {
        QJsonObject invoice;
        invoice["id"]=query->value(0).toString();
        invoice["date"]=query->value(1).toString();
        invoices.append(invoice);
    }
    table["Data"]=invoices;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::invoice_stockHandler()
{
    QJsonObject tab;
    tab["Name"] = "invoice_stock";
    QJsonArray table;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `invoice_stock`");
    while (query->next()) {
        QJsonObject row;
        row["id_invoice"]=query->value(0).toString();
        row["id_ingredient"]=query->value(1).toString();
        row["amount"]=query->value(2).toString();
        row["id_stock"]=query->value(3).toString();
        table.append(row);
    }
    tab["Data"]=table;
    QJsonDocument doc;
    doc.setObject(tab);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::menuTableHandler()
{
    QJsonObject table;
    table["Name"] = "menu";
    QJsonArray menus;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `menu`");
    while (query->next()) {
        QJsonObject menu;
        menu["id"]=query->value(0).toString();
        menu["date"]=query->value(1).toString();
        menu["id_type"]=query->value(2).toString();
        menu["amount"]=query->value(3).toString();
        menus.append(menu);
    }
    table["Data"]=menus;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::menu_dishTableHandler()
{
    QJsonObject tab;
    tab["Name"] = "menu_dish";
    QJsonArray table;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `menu_dish`");
    while (query->next()) {
        QJsonObject row;
        row["id_menu"]=query->value(0).toString();
        row["id_dish"]=query->value(1).toString();
        table.append(row);
    }
    tab["Data"]=table;
    QJsonDocument doc;
    doc.setObject(tab);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::roleTableHandler()
{
    QJsonObject table;
    table["Name"] = "role";
    QJsonArray roles;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `role`");
    while (query->next()) {
        QJsonObject role;
        role["id"]=query->value(0).toString();
        role["role"]=query->value(1).toString();
        roles.append(role);
    }
    table["Data"]=roles;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::stockTableHandler()
{
    QJsonObject table;
    table["Name"] = "stock";
    QJsonArray stocks;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `stock`");
    while (query->next()) {
        QJsonObject stock;
        stock["id"]=query->value(0).toString();
        stock["title"]=query->value(1).toString();
        stocks.append(stock);
    }
    table["Data"]=stocks;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}

QString GetRequestHandler::type_menuTableHandler()
{
    QJsonObject table;
    table["Name"] = "type_menu";
    QJsonArray types;
    QSqlQuery* query = new QSqlQuery(*DB_);
    query->exec("SELECT * FROM `type_menu`");
    while (query->next()) {
        QJsonObject type;
        type["id"]=query->value(0).toString();
        type["title"]=query->value(1).toString();
        types.append(type);
    }
    table["Data"]=types;
    QJsonDocument doc;
    doc.setObject(table);
    delete query;
    return QString(doc.toJson());
}
