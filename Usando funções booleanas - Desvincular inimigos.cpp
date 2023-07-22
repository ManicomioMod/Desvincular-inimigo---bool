1. Usando funções booleanas

Larguei um jogo e vi esta função para obter saúde ilimitada, mas foi compartilhada:
C#:
private int healthPoints(); // 0x436BC7
o que significa que se eu o conectasse e tentasse me dar saúde ilimitada assim:

C++:
int (*old_healthPoints)(void *instance);
int healthPoints(void *instance)
{
    if(instance != NULL) // Check if instance isn't NULL to prevent crashes
    {
        return 999999; // If instance isn't NULL, then make my health 999999
    }
    return old_healthPoints(instance); // If instance is NULL, return original value
}
MsHookFunction((void*)getAbsoluteAddress(0x436BC7), (void*)healthPoints, (void**)&old_healthPoints); // Make sure to call your hooks with MSHook

NÃO IRIA FUNCIONAR E OS INIMIGOS TAMBÉM OBTERÃO SAÚDE ILIMITADA DESDE QUE É COMPARTILHADO!
Então, como podemos desvinculá-lo? Não é o mesmo para todos os jogos, mas tente procurar uma função dentro da mesma classe que você possa usar para desvincular. No meu caso, encontrei esta função que posso usar para desvincular:

C#:
public bool get_isMine(); // 0x453C91
Como faríamos? Bem, nós declaramos um ponteiro de função para ele e é assim que você declara os ponteiros de função:

C++:
FunctionType(*FunctionName)(void *instance, AnyAdditionalParameters);
FunctionName = (FunctionType (*)(void*, TypesOfAdditionalParameters))getAbsoluteAddress(targetLibName, 0xOFFSET);

IMPORTANTE: Dividimos o ponteiro de função assim porque quando sua lib é carregada, às vezes o libil2cpp.so não é carregado, mas sua lib tenta ler o deslocamento de qualquer maneira, o que resulta em uma falha, dividir o ponteiro de função como eu fiz corrigirá isso .
Aqui está o ponteiro de função para get_isMine:

C++:
bool (*get_isMine)(void *instance); // get_isMine doesn't have any additional parameters

int (*old_healthPoints)(void *instance);
int healthPoints(void *instance)
{
    if(instance != NULL) // Check if instance isn't NULL to prevent crashes
    {
        return 999999; // If instance isn't NULL, then make my health 999999
    }
    return old_healthPoints(instance); // If instance is NULL, return original value
}
MsHookFunction((void*)getAbsoluteAddress(0x436BC7), (void*)healthPoints, (void**)&old_healthPoints); // Make sure to call your hooks with MSHook

get_isMine = (bool (*)(void*))getAbsoluteAddress(targetLibName, 0x453C91); // Again, get_isMine doesn't take any additional parameters

Agora que temos um ponteiro de função para get_isMine, podemos criar uma variável para ele e usá-lo para desvincular nosso hack assim:

C++:
bool (*get_isMine)(void *instance); // get_isMine doesn't have any additional parameters

int (*old_healthPoints)(void *instance);
int healthPoints(void *instance)
{
    if(instance != NULL) // Check if instance isn't NULL to prevent crashes
    {
        bool isMine = get_isMine(instance); // Declare a variable to our function pointer
        if(isMine) // Check if it's you
        {
        return 999999; // And if it's you, then make my health 999999 (Now only you get unlimited health)
        }
    }
    return old_healthPoints(instance); // If instance is NULL OR IF IT'S NOT YOU, return original value
}
MsHookFunction((void*)getAbsoluteAddress(0x436BC7), (void*)healthPoints, (void**)&old_healthPoints); // Make sure to call your hooks with MSHook

get_isMine = (bool (*)(void*))getAbsoluteAddress(targetLibName, 0x453C91); // Again, get_isMine doesn't take any additional parameters

