Controles:

Teclado:
a -> anda para a esquerda (na dire��o do vertor U da c�mera)
s -> anda para a direita (na dire��o do vertor U da c�mera)
d -> anda para tr�s (na dire��o do vertor N da c�mera)
w -> anda para frente (na dire��o do vertor N da c�mera)
r -> sobe (na dire��o do vertor V da c�mera)
f -> desce (na dire��o do vertor V da c�mera)

Mouse:
eixo x -> rotaciona a c�mera para os lados (rotaciona em torno do vetor V da c�mera)
eixo y -> rotaciona a c�mera para cima e para baixo (rotaciona em torno do vetor U da c�mera)



Esquema da c�mera:

     |V
     |
     |
   __|_
  |  | |
  |  o---------- U
  |_/__|
   /    Plano de vista
  /
 /N




cesarlimap: -

u = (intersection.getY() - this->yMin->getPoint().getY()) /
                                       (this->yMax->getPoint().getY() - this->yMin->getPoint().getY());
                               v = (intersection.getZ() - this->zMax->getPoint().getZ()) /
                                       (this->zMin->getPoint().getZ() - this->zMax->getPoint().getZ());




tipo...esse eh um de tres casos... eh uma interpola��o (axo q eh esse o nome)..
Sent at 8:08 PM on Tuesday
cesarlimap: pra achar o u e o v q ser�o usados pra mapear na matriz de pixels da imagem... s� n�o estou entendendo quando usa as coordenadas y z ou x z ou x y pra fazer isso