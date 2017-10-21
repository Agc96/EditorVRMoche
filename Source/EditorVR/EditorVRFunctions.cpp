// Hecho por el grupo de Desarrollo de Programas 2 "VR Moche".
// Copyright (C) 2017 Pontificia Universidad Católica del Perú.
// Todos los derechos reservados.

#include "EditorVRFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Función de serialización.
// Guarda en un archivo binario los objetos editables creados con el Editor de Niveles, además de la ubicación del personaje en la escena.
void UEditorVRFunctions::SerializeLevel()
{
	ofstream file("C:/temp/EditorVRMoche/test.bin", ios::out | ios::binary);
	if (file)
	{
		//Escribir el "file signature"
		file.write("VRMoche", 8*sizeof(char));

		//Escribir la posición del personaje
		float player_x = 0, player_y = 0, player_z = 100;
		file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
		file.write(reinterpret_cast<const char*>(&player_y), sizeof(player_y));
		file.write(reinterpret_cast<const char*>(&player_z), sizeof(player_z));

		//Obtener los objetos editables
		//TSubclassOf<AActor> classToFind = AActor::StaticClass();
		//TArray<AActor*> foundEnemies;
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundEnemies);
		int editable_objects = 4;

		//Escribir los datos de los objetos editables
		for (int i = 0; i < editable_objects; i++)
		{
			//Escribir el nombre de la clase
			char* name = "EditableCube";
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));

			//Escribir la posición del objeto
			float x = 1, y = 2, z = 3, pitch = 0.1, yaw = 0.2, roll = 0.3, scale = 10;
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));

			//Escribir la rotación del objeto
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));

			//Escribir la escala del objeto
			file.write(reinterpret_cast<const char*>(&player_x), sizeof(player_x));
		}
		//Cerrar el archivo
		file.write("Nice...", 8 * sizeof(char));
		UE_LOG(LogTemp, Warning, TEXT("Todo ok"));
		file.close();
	}
	else UE_LOG(LogTemp, Error, TEXT("Ggwp"));
}

// Función de deserialización.
// Toma un archivo binario y lo lee para ubicar los objetos editables creados con anterioridad, además de ubicar al personaje en la escena.
void UEditorVRFunctions::DeserializeLevel()
{

}
