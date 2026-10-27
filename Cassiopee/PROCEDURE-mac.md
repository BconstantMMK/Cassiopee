# Compilation de Cassiopee sur macOS Apple Silicon

Cette procédure décrit le portage validé sur macOS arm64 avec GCC Homebrew 14, Python 3.11 et une installation 64 bits dans `Dist/bin/mac_i8`. CPlot et OCC ne sont pas inclus.

## 1. Prérequis système

Installer les outils de ligne de commande Apple, Homebrew GCC 14 et un environnement Python :

```zsh
xcode-select --install
brew install gcc@14

python3.11 -m venv ../.venv
source ../.venv/bin/activate
python -m pip install --upgrade pip setuptools wheel
brew install open-mpi
python -m pip install numpy scons mpi4py
```

Le support MPI combine les en-têtes et bibliothèques natifs Open MPI utilisés par les extensions C/C++ et `mpi4py` pour l'interface Python. Utiliser la même implémentation MPI partout : ici `mpi4py` est lié à Open MPI 4.1.5, installé par Homebrew. `mpicc --showme:compile` et `mpicc --showme:link` permettent de vérifier les chemins et bibliothèques de cette installation. Ne pas remplacer seulement `mpi4py` par une variante MPICH en conservant les extensions compilées contre Open MPI.

Le script d’environnement recherche d’abord un environnement `../.venv/bin/python`, puis `~/cassiopee-venv/bin/python`. Il faut donc créer l’environnement à l’un de ces emplacements, ou définir `CASSIOPEE_PYTHON` avant de sourcer le script. La procédure fournie règle les chemins Python 3.11 dans `PYTHONPATH`; conserver Python 3.11 pour cette configuration.

## 2. Environnement de compilation

Depuis le répertoire du dépôt qui contient `MODULES`, `install` et les dossiers de modules :

```zsh
source Envs/env_Cassiopee_mac
```

Le script définit `CASSIOPEE`, `ELSAPROD=mac_i8`, `PYTHONEXE`, `PRODMODE=1`, les chemins Homebrew et le SDK macOS actif. Le suffixe `_i8` active `E_DOUBLEINT`/`G_DOUBLEINT` côté C/C++ et les entiers 64 bits côté Fortran; la configuration compilée est celle de la clé `mac` de `installBaseUser.py`. `PRODMODE=1` utilise pip pour installer les paquets dans la production, au lieu de l’ancien mode egg. Le wrapper Python reste sélectionné par `PYTHONEXE`, même si le terminal n’a pas de commande `python` globale. Les chemins de la production suivent `CASSIOPEE_PROD`; pour revenir au mode 32 bits, définir `CASSIOPEE_PROD=mac` avant de sourcer le script.

Les wrappers de `Envs/mac-gcc-bin` exposent les noms exacts `gcc`, `g++`, `gfortran`, `ar` et `ranlib` attendus par Cassiopee. Ils sélectionnent GCC/GFortran 14 et transmettent le SDK de `xcrun` aux compilateurs. Le wrapper `ar` délègue les archives ordinaires à l’archiveur Apple, traduit le format MRI des scripts SCons vers `libtool -static`, et gère les modules dont certaines tranches d’archive sont vides.

Vérifier l’environnement avant le build :

```zsh
printf 'CASSIOPEE=%s\nELSAPROD=%s\nPYTHONEXE=%s\nSDKROOT=%s\n' \
  "$CASSIOPEE" "$ELSAPROD" "$PYTHONEXE" "$SDKROOT"
gcc --version
g++ --version
gfortran --version
"$PYTHONEXE" -c 'import numpy, SCons, mpi4py; print(numpy.__version__, mpi4py.__version__)'
```

Les trois compilateurs doivent afficher GCC 14. Vérifier également MPI :

```zsh
mpicc --showme:compile
mpicc --showme:link
"$PYTHONEXE" -c 'from mpi4py import MPI; print(MPI.get_vendor(), MPI.Get_library_version().splitlines()[0])'
```

Le script est prévu pour être sourcé depuis zsh, le shell par défaut de macOS.

## 3. Modules et installation

`MODULES` installe les modules suivants, dans l’ordre requis :

```text
KCore XCore Converter Geom Transform Generator Post Initiator Connector
Distributor2 Dist2Walls RigidMotion Compressor Modeler Intersector Apps Roms
```

CPlot et OCC sont exclus de cette production. Une fois l’environnement sourcé, lancer depuis la racine `Cassiopee/` :

```zsh
./install
```

Les fichiers sont installés dans `Dist/bin/mac_i8`. Le build peut afficher des avertissements de dépendances optionnelles absentes, notamment ADOL-C; ils n’empêchent pas la production sans ces fonctions. MPI est maintenant détecté pour les extensions qui le prennent en charge. La lecture/écriture HDF5 parallèle dépend en plus d'une construction HDF5 avec support MPI; la bibliothèque HDF5 Homebrew standard peut rester séquentielle.

Pour reconstruire un module après une modification, utiliser son script local, par exemple :

```zsh
cd Connector
./install
cd ..
```

## 4. Validation

Vérifier les imports depuis la racine de build, afin d’éviter que le répertoire source d’un module masque le paquet installé :

```zsh
"$PYTHONEXE" -c 'import Converter.PyTree as C; import Connector.IBM as X_IBM; import Distributor2.PyTree as D2; print("OK")'

cd Connector/test
"$PYTHONEXE" IBMrectilinear_t1.py
```

Pour les tests de la suite, respecter leur suffixe : les cas `_m*.py` se lancent avec exactement deux rangs MPI (`kpython -n2 ...`), les cas `_t*.py` avec un seul rang (`kpython -n1 ...`). Le cas `Generator/test/adaptMeshPT_m1.py` a ainsi été validé à deux rangs, et `generateIBMMeshPT_t1.py` à un rang. Les références de test dépendent du nombre de rangs; ne pas réutiliser les références créées par un lancement diagnostique avec une autre topologie.

Valider également Open MPI sur deux rangs :

```zsh
mpiexec -n 2 "$PYTHONEXE" -c 'from mpi4py import MPI; import Converter.Mpi as Cmpi; c=MPI.COMM_WORLD; print(c.rank, c.size, MPI.get_vendor()[0], Cmpi.size); assert c.allreduce(1, op=MPI.SUM) == 2'
```

Les extensions `Converter`, `XCore` et `Intersector` ont été reconstruites avec MPI natif; Converter active aussi l’interface mpi4py. L’exécution de cette commande à deux rangs a été validée avec Open MPI 4.1.5.

## 5. Adaptations de portage incluses

- `KCore/installBaseUser.py` déclare la production `mac` sans modifier le dictionnaire partagé `KCore/installBase.py`. Les options activent OpenMP, ajoutent les chemins Homebrew nécessaires et désactivent la variante NEON du PNG embarqué, dont les objets d’implémentation ne sont pas compilés par cette chaîne.
- `KCore/Dist.py` ne force plus `-dynamiclib` sur les extensions macOS. Setuptools utilise déjà `-bundle` pour les modules Python, option incompatible avec `-dynamiclib` sous GCC.
- `KCore` et les autres bibliothèques utilisent des archives statiques Apple valides. Les scripts MRI existants sont traduits localement par `Envs/mac-gcc-bin/ar`.
- ADF sélectionne `lseek64` seulement sur Linux/Windows et utilise `open` sans `O_NOATIME` lorsqu’il n’existe pas, comme sur macOS.
- SCOTCH active ses réglages macOS lorsque `__APPLE__` est défini, notamment l’inclusion de `sys/time.h`.
- En mode `mac_i8`, le sentinel `IDX_NONE` est converti explicitement en `E_Int` dans `NGON_BooleanOperator.h`; sans ce typage, GCC voit un appel ambigu entre les constructeurs de tableau prenant une valeur initiale ou un booléen.
- Le crash MPI de `Generator/test/adaptMeshPT_m1.py` venait de PT-Scotch : `SCOTCH_Dmapping` est un objet opaque dont la taille publique était inférieure à `LibDmapping` lorsque `SCOTCH_PTHREAD` était activé. `XCore/XCore/scotch/ptscotch.h` réserve désormais un tampon suffisant dans cette configuration, et `XCore/setup.scons` transmet `-DG_DOUBLEINT` à la bibliothèque PT-Scotch pour aligner l'interface sur la production `mac_i8`. Sans cette correction, `SCOTCH_dgraphMapInit` signalait une erreur interne puis `SCOTCH_dgraphPart` provoquait un crash natif.
- Les conversions `E_Int` vers Python dans Connector et Distributor2 utilisent des entiers Python explicites. Dans l'API C de Python, le code de format `"l"` de `Py_BuildValue` signifie toujours « argument de type C `long` »; ce n'est pas un alias de `E_Int` et Cassiopee ne le redéfinit pas selon la production. Pour `ELSAPROD=mac`, `E_Int` est un `int` 32 bits, tandis que macOS arm64 suit l'ABI LP64 où `long` fait 64 bits. Passer cet `int` à une fonction variadique qui lit un `long` est un comportement indéfini et a produit des indices corrompus. En mode `mac_i8`, le type interne change, mais le code `"l"` reste le type C `long`; il ne faut donc pas s'en servir comme conversion générique de `E_Int`.
- La production `mac_i8` complète et le test IBM ont été validés. GCC signale encore des avertissements de formats comme `%ld` lorsque le code lui passe un `E_Int` de type `long long`; ils ne bloquent pas ce build, mais la variante 64 bits ne les corrige pas automatiquement. Les appels concernés devront être audités séparément avant de considérer l’ensemble des interfaces C/Python et des formats de fichiers comme validés en int64.
- Roms n’essaie plus de lier `libroms`, car `Roms/srcs.py` ne déclare aucune source native pour cette bibliothèque.
- `MODULES` exclut CPlot et OCC pour la production macOS de base.
- Le support MPI utilise une seule implémentation cohérente, Open MPI Homebrew 4.1.5 : `mpi4py` et `mpicc` sont liés au même runtime. `KCore/Dist.py` reconnaît désormais `libmpi.dylib`; les chemins des headers et bibliothèques Open MPI sont fournis par `KCore/installBaseUser.py` et l’environnement macOS.

Les scripts d’environnement et wrappers sont locaux au checkout. Si Homebrew GCC 14 ou Python sont installés à un autre emplacement, définir `CASSIOPEE_GCC_BIN` ou `CASSIOPEE_PYTHON` avant de sourcer `Envs/env_Cassiopee_mac`.
