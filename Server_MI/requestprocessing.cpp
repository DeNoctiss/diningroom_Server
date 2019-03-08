#include "requestprocessing.h"

RequestProcessing::RequestProcessing(qintptr socket_id, QSqlDatabase*  db): Socket_id(socket_id),DB_(db){
    qDebug() << "client "+ QString::number(socket_id) +" connected";
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void RequestProcessing::run(){
    Socket_ = new QTcpSocket;
    Socket_->setSocketDescriptor(Socket_id);
    Socket_->waitForReadyRead(1000);
    Responce();
    Socket_->waitForBytesWritten(1000);
    Socket_->disconnectFromHost();
    Socket_->close();
    Socket_->deleteLater();
    delete Socket_;
}


void RequestProcessing::Responce(){
    QString ask(Socket_->readAll());
    if(ask.isEmpty()) return;
    qDebug() << ask;
    Request_=new Request(ask);
    if(Request_->GetType()=="GET"){
        GetRequest();
    }
    if(Request_->GetType()=="POST"){
        PostRequest();
    }
}

void RequestProcessing::GetRequest(){
    GetRequestHandler handler(DB_,Request_);
    QString response;
    if(Request_->GetPath()=="/dish.json")
        response=handler.dishHandle();
    if(Request_->GetPath()=="/stock_ingredients.json")
        response=handler.StockIngHandler();
    if(Request_->GetPath()=="/menu.json")
        response=handler.menuHandler();
    if(Request_->GetPath()=="/auth.json")
        response=handler.authHandler();
    if(Request_->GetPath()=="/invoice.json")
        response=handler.invoiceHandler();
    if(Request_->GetPath()=="/wkinfo.json")
        response=handler.wkInfoHandler();
    if(Request_->GetPath()=="/gueststable.json")
        response=handler.guestsTableHandler();
    if(Request_->GetPath()=="/countguests.json")
        response=handler.guestCountHandler();
    if(Request_->GetPath()=="/authtable.json")
        response=handler.authTableHandler();
    if(Request_->GetPath()=="/dishtable.json")
        response=handler.dishTableHandler();
    if(Request_->GetPath()=="/ingredienttable.json")
        response=handler.ingredientsTableHandler();
    if(Request_->GetPath()=="/ingredients_dishtable.json")
        response=handler.ingredients_dishTableHandler();
    if(Request_->GetPath()=="/ingredients_stocktable.json")
        response=handler.ingredients_stockHandler();
    if(Request_->GetPath()=="/invoicetable.json")
        response=handler.invoiceTableHandler();
    if(Request_->GetPath()=="/invoice_stocktable.json")
        response=handler.invoice_stockHandler();
    if(Request_->GetPath()=="/menutable.json")
        response=handler.menuTableHandler();
    if(Request_->GetPath()=="/menu_dishtable.json")
        response=handler.menu_dishTableHandler();
    if(Request_->GetPath()=="/roletable.json")
        response=handler.roleTableHandler();
    if(Request_->GetPath()=="/stocktable.json")
        response=handler.stockTableHandler();
    if(Request_->GetPath()=="/type_menutable.json")
        response=handler.type_menuTableHandler();
    if(response.isEmpty()){
        Socket_->write("HTTP/1.1 404 \r\n\r\nBad request");
    }
    else {
        response ="HTTP/1.1 200 OK \r\nContent-Type: application/json\r\n\r\n" + response;
        Socket_->write(response.toLocal8Bit());
    }

}

void RequestProcessing::PostRequest(){
    PostRequestHandler handler(DB_,Request_);
    QString response;
    if(Request_->GetPath()=="/register.json")
        response=handler.registrHandler(Request_->GetPost());
    if(Request_->GetPath()=="/newingredient.json")
        response=handler.newIngredientHandler(Request_->GetPost());
    if(Request_->GetPath()=="/newstock.json")
        response=handler.newStockHandler(Request_->GetPost());
    if(Request_->GetPath()=="/newdish.json")
        response=handler.newDishHandler(Request_->GetPost());
    if(Request_->GetPath()=="/newguest.json")
        response=handler.newGuestHandler(Request_->GetPost());
    if(Request_->GetPath()=="/updateguest.json")
        response=handler.updateGuestHandler(Request_->GetPost());
    if(Request_->GetPath()=="/addmenu.json")
        response=handler.addMenuHandler(Request_->GetPost());
    if(Request_->GetPath()=="/addinvoice.json")
        response=handler.addInvoiceHandler(Request_->GetPost());
    if(Request_->GetPath()=="/delete.json")
        response=handler.deleteTableHandler(Request_->GetPost());
    if(Request_->GetPath()=="/deletebyid.json")
        response=handler.deleteByIdHandler(Request_->GetPost());
    if(Request_->GetPath()=="/deletebywhere.json")
        response=handler.deleteByWhereHandler(Request_->GetPost());
    if(Request_->GetPath()=="/insert.json")
        response=handler.insertHandler(Request_->GetPost());
    if(response.isEmpty()){
        Socket_->write("HTTP/1.1 404 \r\n\r\nBad request");
    }
    else {
        response ="HTTP/1.1 200 OK \r\nContent-Type: application/json\r\n\r\n" + response;
        Socket_->write(response.toLocal8Bit());
    }
}

RequestProcessing::~RequestProcessing(){

}
