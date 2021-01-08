#include <iostream>
#include <SQLAPI.h>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Método que borra y crea las tablas, e inserta 10 tuplas en la tabla stock
void crearBorrarTabla(SAConnection &con, SACommand &cmd){

    cout << "Borrando tablas: " << endl;

    cmd.setCommandText(
    _TSA("DROP TABLE DETALLE_PEDIDO"));
    cmd.Execute();

    cmd.setCommandText(
    _TSA("DROP TABLE STOCK"));
    cmd.Execute();

    cmd.setCommandText(
    _TSA("DROP TABLE PEDIDO"));
    cmd.Execute();

    // Crear tablas.

    cout << "Creando tabla STOCK: " << endl;
    cmd.setCommandText(
    _TSA("CREATE TABLE STOCK (CProducto INTEGER GENERATED ALWAYS AS IDENTITY, CANTIDAD INTEGER DEFAULT 0, PRIMARY KEY (CProducto))"));
    cmd.Execute();


    cout << "Creando tabla PEDIDO: " << endl;
    cmd.setCommandText(
    _TSA("CREATE TABLE PEDIDO( CPedido INTEGER GENERATED ALWAYS AS IDENTITY, Ccliente INTEGER, Fecha_pedido DATE DEFAULT SYSDATE, PRIMARY KEY(Cpedido) )"));
    cmd.Execute();

    cout << "Creando tabla DETALLE_PEDIDO: " << endl;
    cmd.setCommandText( "CREATE TABLE DETALLE_PEDIDO(CProducto INTEGER, CPedido INTEGER, Cantidad INTEGER DEFAULT 0,PRIMARY KEY (CProducto, CPedido), CONSTRAINT CprimariaProducto FOREIGN KEY (CProducto) REFERENCES STOCK (Cproducto), CONSTRAINT CprimariaPedido FOREIGN KEY (CPedido) REFERENCES PEDIDO (CPedido) )" );
    cmd.Execute(); 

    cout << "Insertando tuplas en tabla STOCK: " << endl;

    cmd.setCommandText(
    _TSA("Insert into STOCK (CANTIDAD) values (:1)"));

    for (int i = 0; i < 10; ++i)
    {
        cmd << (long)(rand() % 100);
        cmd.Execute();
        cout << i+1 << " filas insertadas" << endl;
    }


    con.Commit();
}

// Método que muestra la tabla stock
bool mostrarTablaStock(SACommand &select)
{
	cout.setf(ios::fixed);

    select.setCommandText(_TSA("SELECT * FROM STOCK"));
	select.Execute();

	cout << "| " << setw(9) << "CProducto" << " |" 
		 <<	"| " << setw(9) << "Cantidad" <<  " |" << endl 
		 <<	"|" << "___________" <<  "|"   
		 <<	"|" << "___________" <<  "|" << endl; 

	while(select.FetchNext())
	{
		for(int i = 1; i <= select.FieldCount(); ++i)
		{
			cout << "| " << setw(9) <<select[i].asLong() << " |";
		}

		cout << endl;
	}

	cout << endl;

	return select.isResultSet();
}

// Método que muestra la tabla pedido
bool mostrarTablaPedido(SACommand &select)
{
	cout.setf(ios::fixed);

    select.setCommandText(_TSA("SELECT * FROM PEDIDO"));
	select.Execute();
	
	
	cout << "| " << setw(9) << "CPedido" << " |" 
		 <<	"| " << setw(9) << "CCliente" <<  " |"
		 <<	"| " << setw(13) << "Fecha" <<  setw(9) << " |" << endl 
		 <<	"|" << setw(9) << "___________" <<  "|"   
		 <<	"|" << setw(9) << "___________" <<  "|"   
		 <<	"|" << setw(20) << "______________________" <<  "|" << endl; 

	while(select.FetchNext())
	{
		cout << "| " << setw(9) <<select[1].asLong() << " |"
			 << "| " << setw(9) <<select[2].asLong() << " |"
			 << "| " << setw(20) << (string)select[3].asString() << " |" << endl;
	}

	cout << endl;

	return select.isResultSet();
}

bool mostrarTablaDetallePedido(SACommand &select)
{

    select.setCommandText(_TSA("SELECT * FROM DETALLE_PEDIDO")); 
	select.Execute();

	while(select.FetchNext()) {
		for(int i = 1; i <= select.FieldCount(); ++i)
		{
			cout << "| " <<select[i].asLong() << " |";
		}

		cout << endl;
	}

	cout << endl;

	return select.isResultSet();
}


int main(int argc, char* argv[])
{
    SAConnection con; // connection object
    SACommand cmd;    // Create a command object 
    SACommand select;

    try{
        // Connect to database
        con.Connect(_TSA("oracle0.ugr.es:1521/practbd.oracle0.ugr.es"),
                _TSA("x7149055"), 
                _TSA("x7149055"), 
                SA_Oracle_Client);
        
        cout << "Conectados" << endl;

        // Asociamos un comando a la conexión
        cmd.setConnection(&con);
		select.setConnection(&con);

        bool salir = false; 
        int entrada;
        while (!salir)
        {

            cout << endl << "MENU" << endl
                << "1  Borrado/Creación de las tablas" << endl
                << "2  Dar de alta nuevo pedido" << endl
                << "3  Borrar un pedido" << endl
                << "4  Salir del programa" << endl
                << endl << "Opcion seleccionada: ";

            cin >> entrada;

            switch (entrada){
            case 1:
                crearBorrarTabla(con, cmd);
                break;

            case 2:
				{
					bool salir2 = false;
					int codCliente;

					cout << "Introduce el código de cliente al cual asignar el pedido: ";
					cin >> codCliente;
					cout << endl;

					cmd.setCommandText(
						_TSA("Insert into PEDIDO (Ccliente) values (:1)"));
					cmd << (long)codCliente;
					cmd.Execute();

					while(!salir2){
						cout << "1  Añadir detalle de producto" << endl
							<< "2  Eliminar todos los detalles de pedido" << endl
							<< "3  Eliminar pedido" << endl
							<< "4  Finalizar" << endl
							<< endl
							<< "Opcion seleccionada: ";
						cin >> entrada;
						cout << endl;
					
						switch (entrada){

							case 1:
								{ // Estos corchetes son necesarios porque estamos inicializando variables en 
								  // este case
								  
									int cprod;
									int cpedido;
									unsigned cantidad       , 
											 cantidad_stock ;

									bool preguntar = true;
									
									// Creación de un punto de guardado antes de introducir los detalles del pedido
									cmd.setCommandText(_TSA("SAVEPOINT detalles_pedido"));
									cmd.Execute();

									// Obtenemos el código del pedido que se está realizando
									// que será el código del último pedidi introducido en la tabla PEDIDO
									select.setCommandText(_TSA("SELECT MAX(CPedido) FROM PEDIDO"));
									select.Execute();

									while(select.FetchNext())
										cpedido = select[1].asLong();
									
									// Mostramos la tabla STOCK
									cout << endl << " TABLA STOCK" << endl;
									mostrarTablaStock(select);


									// Solicitamos los detalles del pedido
									cout << "Código del producto que deseas pedir: ";
									cin >> cprod;
									cout << endl;
									
									while(preguntar){

										cout << "Introduzca la cantidad : ";
										cin >> cantidad;
										cout << endl;

										//Comprobamos que la cantidad deseada sea menor o igual a la cantidad de stock
										select.setCommandText("SELECT cantidad FROM STOCK WHERE CProducto = :1");
										select << (long)cprod;
										select.Execute();

										
										while(select.FetchNext())
											cantidad_stock = select[1].asLong();
										

										if(cantidad > cantidad_stock)
											cout << "La cantidad deseada es mayor a la cantidad en stock" << endl;
										else
											preguntar = false;

									}


									// Insertamos los detalles
									cmd.setCommandText(
										_TSA("Insert into DETALLE_PEDIDO (CProducto, CPedido, Cantidad) values (:1, :2, :3)"));
									cmd << (long)cprod;
									cmd << (long)cpedido;
									cmd << (long)cantidad;
									cmd.Execute();

									// Modificamos el stock del producto seleccionado
									cmd.setCommandText(
											_TSA("UPDATE STOCK SET Cantidad = :1 WHERE CProducto = :2"));
									cmd << long(cantidad_stock - cantidad);
									cmd << long(cprod);
									cmd.Execute();

									break;
								}//case 1

							case 2:
								// Rollback al punto antes de introducir los detalles de pedido
								cmd.setCommandText(
									_TSA("ROLLBACK TO detalles_pedido"));
								cmd.Execute();
								break;
								

							case 3:
								// Volvemos al último commit
								// que será antes de realizar el pedido
								con.Rollback();                        
							case 4:
								con.Commit();
								cout << "\nGuardando cambios..." << endl;
								salir2 = true;
								break;

							default:
								cout << endl << "Opción Incorrecta";
								break;
						}//switch
					}// while 

					break;
				}//case 2

            case 3:

				cout << "TABAL PEDIDO" << endl;
				if(mostrarTablaPedido(select))
				{
					cout << endl << "Ingresa el número de pedido que deseas eliminar: ";
					cin >> entrada;
					
					cmd.setCommandText(
					_TSA("delete FROM DETALLE_PEDIDO where CPedido = :1"));
					cmd << (long)entrada;
					cmd.Execute();

					cmd.setCommandText(
					_TSA("delete FROM PEDIDO where CPedido = :1"));
					cmd << (long)entrada;
					cmd.Execute();

					con.Commit();
				}
				else
					cout << "No hay pedidos" << endl;

                break;

            case 4:
                salir = true;
                break;

            default:
                cout << endl << "Opción Incorrecta";
                break;
            }

            
        }

        con.Disconnect();
        cout << "Desconectando de la base de datos" << endl;
	}
    catch(SAException &x)
    {
        // SAConnection::Rollback()
        // can also throw an exception
        // (if a network error for example),
        // we will be ready
        try
        {
            // on error rollback changes
            con.Rollback();
        }
        catch(SAException &)
        {
        }
        // print error message
        printf("%s\n", x.ErrText().GetMultiByteChars());  
    }
   
   return 0;
}
