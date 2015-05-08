% validações
vcruz(N,Tab)  :- arg(N,Tab,V), nonvar(V), V=x.
vbola(N,Tab)  :- arg(N,Tab,V), nonvar(V), V=o.
vazia(N,Tab)  :- arg(N,Tab,V), var(V).
cheia(N,Tab)  :- arg(N,Tab,V), nonvar(V).
str(X):- var(X)->write(' '); write(X).

criatab(X) :- X =  tab(_,_,_,_,_,_,_,_,_,_).

valida([1,2,3]).
valida([4,5,6]).
valida([7,8,9]).
valida([1,4,7]).
valida([2,5,8]).
valida([3,6,9]).
valida([1,5,9]).
valida([3,5,7]).

venceu(cruz,jogador1).
venceu(bola,jogador2).

% faz a troca dos players
proximo(jogador1,jogador2).
proximo(jogador2,jogador1).

% define o simbolo do jogador
jogador(jogador1,x).
jogador(jogador2,o).

%desenho de tabuleiro
desenhatab(T) :- nl,
tab(7),linha(1,2,3,T),
tab(7),write('------'),nl,
tab(7),linha(4,5,6,T),
tab(7),write('------'),nl,
tab(7),linha(7,8,9,T).

% escreve a linha
linha(X,Y,Z,T):-arg(X,T,V1), str(V1),write('|'),
arg(Y,T,V2), str(V2),write('|'),arg(Z,T,V3), str(V3),nl.

% preenche todos os indices para verificar empate
preenche(XO,T):- L = [1,2,3,4,5,6,7,8,9],member(X,L),vazia(X,T),arg(X,T,XO),!,preenche(XO,T).

% verifica se se o jogo acabou, se tiver vencedor ou empate
fim(T,empate) :- empate(T).
fim(T, R) :- vence(T,X),venceu(X,R).

vence(T,cruz):- valida([A,B,C]), vcruz(A,T),vcruz(B,T),vcruz(C,T),!.
vence(T,bola):- valida([A,B,C]), vbola(A,T),vbola(B,T),vbola(C,T),!.
empate(T):- preenche(o,T),\+ vence(T,_),!,preenche(x,T),\+ vence(T,_).

% le a jogada
jogada(T, Player):- write('Escolha uma posição (1..9): '),write(Player), nl , read(P),validajogada(P,T,Player).

% verifica se jogada é valida
validajogada(P,Tab,Player) :- vazia(P,Tab), jogador(Player,C), arg(P,Tab,C),!;
write('Jogada invalida!'),nl, escolheMov(Tab,Player).

jogar(T, _):- fim(T,Result),!,write('Vencedor: '), write(Result),nl,nl.
jogar(T, Jogador):- jogada(T, Jogador),!,
desenhatab(T),!,
proximo(Jogador, Oponente), !,
jogar(T, Oponente).

% inicio
jogovelha :- criatab(T),
              desenhatab(T), jogar(T, jogador1),write('Fim do Jogo!').