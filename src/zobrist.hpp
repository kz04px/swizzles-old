#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <cstdint>

struct Position;

namespace zobrist {

namespace keys {

const std::uint64_t flipped = 0xa76c5a9b990e89a7ULL;
const std::uint64_t castling[2][2] = {
    0x6cf916069491ae6ULL,
    0xa696a2a079a075c6ULL,
    0xe27b282b1f1125aULL,
    0x8c0891b6e416fdb1ULL,
};
const std::uint64_t enpassant[8] = {
    0xdb27165ccf23595cULL,
    0xa2509f25ed6d766fULL,
    0x877fa40bfcc07cfcULL,
    0x2fa3d9c3ca6d94bdULL,
    0xd4b68f989e5804c9ULL,
    0x122e07022bf1bd45ULL,
    0x5a944ee527f8dd8eULL,
    0xfda5d5d90bc75daeULL,
};
const std::uint64_t piece[64][6][2] = {
    0x113a0784155583f2ULL, 0x82aabe7652003889ULL, 0x8ba68203d3619311ULL,
    0x259755d088be7748ULL, 0x834c39366c1b37a2ULL, 0x502a6dc7f547401cULL,
    0x1544606c9082ce5fULL, 0x72f29e7a685f398eULL, 0x7ad95ec297a9840aULL,
    0xdf028a5dfddabe57ULL, 0x5e60f36e0d61205ULL,  0x54dbaa8b8dc5596bULL,
    0xc538a83133075438ULL, 0xdf82720a588be611ULL, 0x4bdcb25610f6a01cULL,
    0x177ce00bac143ddeULL, 0x60e48d0aca4d922fULL, 0xa8e0524ee41e710ULL,
    0xc2c33ff62b5582e7ULL, 0xe3ca9b7496e93e4dULL, 0x1c0070c68723c49eULL,
    0x98b8fa9b11515fd2ULL, 0x965ce164cc2ba6b4ULL, 0x312b34ba7703e99dULL,
    0xfc13d49da8955fe5ULL, 0x71f5529838108dfeULL, 0x69af46d4b14537caULL,
    0xe5b74ab56a5bd176ULL, 0xe9027e1a2325f3eULL,  0xee43864f24a8f847ULL,
    0xae84d2ec10fce47dULL, 0x528a33441b950bf0ULL, 0xbac0a8f45b4d1feeULL,
    0xd56c28078cd3e3c3ULL, 0xc586206456f39cddULL, 0x3237870e25171a7cULL,
    0x58b5a54efee5ddcaULL, 0x7024082049fba2d4ULL, 0x1f05c9dd069c25d5ULL,
    0x414d83c6b5dc2aaaULL, 0xb1ae24129fb3e03eULL, 0xb7ac82a282915019ULL,
    0x52bd81cf99be917fULL, 0xf17b1a51e22220dULL,  0x2ed7f39846ab1123ULL,
    0x3e97adf3b40d1c11ULL, 0x653a464a77e13b40ULL, 0x437f2bb948a40ed3ULL,
    0xd32b551ad61d8f95ULL, 0xda4ad989217ca9ceULL, 0x82de5f776902b08aULL,
    0x11d8078e0a33468bULL, 0x75ef546b6b3df924ULL, 0x6e4123f80e1caca8ULL,
    0x6f421796abc7e6c6ULL, 0x2a71550113770ff9ULL, 0xef0ad3ba3c6624e2ULL,
    0x24ee893cc5e65b33ULL, 0xd868431d04721d8fULL, 0x8de2688d9d995525ULL,
    0x86e4e4dee2a200e0ULL, 0x5833853e59426be5ULL, 0xc8a478a379536332ULL,
    0x83d2c21fd0b1d3ceULL, 0xb94ea18f648b3213ULL, 0x300a3cab22028ec1ULL,
    0xbcbe5d032e10aa64ULL, 0x667a10b8f0af61dbULL, 0xf9e0536412cec8a1ULL,
    0x37ab362c99ad6a76ULL, 0xd7369f9c378ba25fULL, 0xe58f7dc46d886213ULL,
    0x22e554bae3c0d3fcULL, 0xab7af44868dd5c65ULL, 0x662411dbfa477442ULL,
    0xf0b8370e75f5fc2ULL,  0xaebb76e9043b3f2dULL, 0x7e24ceddab3ace68ULL,
    0x30ca1580c6f66623ULL, 0x9e9caf6e7c861a56ULL, 0xef6752f762a26075ULL,
    0xe2e6c83b2e9ce1dcULL, 0x9c447ec42b4abe1cULL, 0x7a42926802ca7451ULL,
    0x5bc0aad8f069d84bULL, 0xbbcb6560da3af299ULL, 0x32be2e848a9461bULL,
    0x68e35c98b8af8027ULL, 0x92dccfa9776e5cf3ULL, 0xf13c503ef118f7afULL,
    0xdb1357a98689f6b4ULL, 0x98b06a884d031c38ULL, 0xd602b6340a0687feULL,
    0x96e12bc144cf4918ULL, 0xc38b04b39881513cULL, 0x8505cca7e560c248ULL,
    0x94d8ccbce5433192ULL, 0xc0f6456943e84035ULL, 0x930f5f4dbfde31b3ULL,
    0xe5e4358b8ac68a33ULL, 0xb4303e5c33709d15ULL, 0xbb53096cafa3f67aULL,
    0xe1f22c7e80be6c9bULL, 0x2fb08a89f701fce9ULL, 0x14e35aa004ad61e1ULL,
    0xfb6e6f0c02618c38ULL, 0x1f8d1ae2764192a5ULL, 0xeabcf556f7b50bb0ULL,
    0xad1a821c5080a5d2ULL, 0xb8c57b032b6311b7ULL, 0x42820f29e34c4ab4ULL,
    0x78b932fc2e0eb622ULL, 0x2fe6c8bebc56999dULL, 0xea2daac8d0d28566ULL,
    0xd6d14556c3592f84ULL, 0x7c65f08214fa7f8dULL, 0x252351f5b8ea5b82ULL,
    0x759beb40e771b90fULL, 0x62ed6863e62a93e7ULL, 0x6a883112ab59d003ULL,
    0xbe52b98272e8e785ULL, 0x195bc1c6a57871b0ULL, 0xeea3a178d2f3b427ULL,
    0x1a990754aac4583ULL,  0xabc4580dc1b0350cULL, 0xdd3dfb792034a216ULL,
    0xb9a185b1484cb8b7ULL, 0x19cb22987ae89ec4ULL, 0xcf36ada834d749cULL,
    0x2e96a37d1aea1a8ULL,  0x77cd1f06402c7112ULL, 0x200ac285f67390e9ULL,
    0x9bebb1dfe1edba4cULL, 0x4c73a3cb159f45f4ULL, 0xa219820e809611c7ULL,
    0xb44f500e176931faULL, 0x6423573d4b0e8c9cULL, 0xacf0ebf6e714b701ULL,
    0x99d34e3d6cfe0f0cULL, 0x61d427fe12da300bULL, 0x94da7fac5d242159ULL,
    0x636fa6cdd48cbfb2ULL, 0x540e458a3f574409ULL, 0x1904820ab4bbf940ULL,
    0x825aac97a84a199cULL, 0xc4a9cc3c6948787ULL,  0xe080308ad337c0ffULL,
    0xf372fff0cb4b65f8ULL, 0xdaa69ed70a5f8138ULL, 0xc7f52d01587c5c9eULL,
    0x34e2de9755120265ULL, 0x98dcce0ad605aee1ULL, 0x1866f9a20495a7a0ULL,
    0xeed7cd335aee8206ULL, 0x51557bd8cd3a3b82ULL, 0xdefd8f9fbe0b7727ULL,
    0x22b3f811d662da1eULL, 0xcb47ec40cc984169ULL, 0x286e447ab358aa66ULL,
    0x36929d24a345cce1ULL, 0x3e15b65af4862eb1ULL, 0x885fcf87fd9d81afULL,
    0xaa937e25ddd912f6ULL, 0x5fbc264b914af5c6ULL, 0x2a87248db5b9a318ULL,
    0x86be4be8551ebf1ULL,  0xc91c49d87b14a3b7ULL, 0xc8775deb8ddfd968ULL,
    0xe0830d2e58a1363ULL,  0x5cedf7fc9ab3af68ULL, 0x32f93bdbf0429414ULL,
    0x65d00299e5a7e92dULL, 0x1029f54966c4471eULL, 0xa6dca3aae05037a3ULL,
    0xa47de8f1ecbf22aeULL, 0xbe5c23c69c07be0cULL, 0xd25be64127b06cdULL,
    0xccf15b8ae0a6e8d2ULL, 0x4e4a4df221f5b465ULL, 0xa46896c0d75b66f5ULL,
    0x1870efdb78b8c52aULL, 0x48aca25763790f3cULL, 0x1d8910f5d7634a4cULL,
    0x53817b1c69b1929bULL, 0x6557e9a3e2c47cffULL, 0xe8179a7f712d7ed6ULL,
    0x89f129ddda3f30b0ULL, 0xe5bd54b3c3fcdb8fULL, 0xbadefe03194f258dULL,
    0x8e0a0b635502fb87ULL, 0x23dee14dfb13e092ULL, 0x7ae97c2ad330f3b7ULL,
    0x9dc1e09b46d1edaULL,  0xff70047ea3fa809cULL, 0x444238e5d869cb5dULL,
    0xd7afcf7ee8d70d88ULL, 0x3f334cc74aef9e7bULL, 0xd061b6f42a13a0d3ULL,
    0x5730d4a61ccb681ULL,  0x2eb0ccc1b3fb6cULL,   0xe17358031c864632ULL,
    0xce9ba88d6120245aULL, 0xd0728d91b56ce694ULL, 0x6f8e25722c57ce22ULL,
    0x105815a2ccd7e5dULL,  0x11bde52123902346ULL, 0xf7eafb02116f0856ULL,
    0x58a22711a7759675ULL, 0xca221e7f7942623cULL, 0xb783864dfbcbd8a2ULL,
    0x59998bb69b973e97ULL, 0x856703f8f270beb0ULL, 0x6389839ec4e9338dULL,
    0x5e34d2815bef6df7ULL, 0x31b9c22df3bee3e3ULL, 0x6e1f4630842193d3ULL,
    0x1e165c76c7571b64ULL, 0x6d3d520b7dd62fd9ULL, 0x3be60da0175c404fULL,
    0xf5b3ac83afa59d39ULL, 0x6e26f0cdfbbe9735ULL, 0x37710d615a57e693ULL,
    0xda820e16b74a9675ULL, 0x318328d92192c8e1ULL, 0x24b62cad8dbf7eb8ULL,
    0x16e7991b83e41c83ULL, 0x34a037d2e12f7512ULL, 0xccb9f52b0b49e6bbULL,
    0x71e0e3f3225e56b3ULL, 0x2bf6de7dda34a066ULL, 0xdd3e0bdec8a67b1bULL,
    0x89b76930cc048e1ULL,  0x7c6336c0f395db26ULL, 0xa5d80cda964d6f29ULL,
    0xd960009425ca5506ULL, 0xc49a5181ad79a96ULL,  0xf9db4c78e64648e9ULL,
    0x22fbf1372e31f568ULL, 0xe884a89e655a969dULL, 0xa3f3ec84ea6af3bbULL,
    0xe7177a1cd2ad4d64ULL, 0x7b493e6dedc6cd9ULL,  0x6fda1daba0af7e32ULL,
    0x8fc9310451d24e7cULL, 0x23d5cbc1b7f99c4cULL, 0xaa32e0d9a60726eeULL,
    0xe2d16bec2eeb67a3ULL, 0xb32a88079cfb1bdfULL, 0xa3b21d6526976b1dULL,
    0x6fb510498124f646ULL, 0x89c0193069336302ULL, 0x672f01a05125c564ULL,
    0x4cbf2b88a29ae33dULL, 0x46f19b385426afe9ULL, 0xac3eac2e5893b8ffULL,
    0x68151bb2fd928acaULL, 0x7638dca5b6eeb399ULL, 0x638df4494546ca9eULL,
    0xc2e6f0e66a40d426ULL, 0xa4c1b894f7c14850ULL, 0xe7e309f0d261876cULL,
    0x212f43bfd74d2e27ULL, 0x39854fe4c2c0ab39ULL, 0x570a00a60de7f80bULL,
    0xb9b4483403bc726bULL, 0x441e87c26825007dULL, 0xb47660e4b16b776fULL,
    0x14f02f8b9c29ec63ULL, 0xfcc96963272b25cbULL, 0xcf2007440aa6dc31ULL,
    0x8d00895f185f2fe4ULL, 0xf68c94f5941ecc2bULL, 0x52a7379e474e733bULL,
    0xd4a3c5454085f689ULL, 0xd73cc4cf7dcd47ULL,   0xbf17d0b8b5537ad8ULL,
    0x7b9855eb18d498a6ULL, 0x46f7df70ad2c140dULL, 0xdadeab2b0bd72febULL,
    0x465ed5ec9a20e52bULL, 0xb73f3841327eb774ULL, 0xf1460c1aa25ee142ULL,
    0x8dbe62b5c080a92dULL, 0x1de95b2d5975f6ccULL, 0x214a24f54fae7bdcULL,
    0x46ec3d3bf1ecb61fULL, 0xcd288cfda25ab488ULL, 0xa0f5498c14b91c02ULL,
    0xd2eb0fd87f419fddULL, 0x8c8e7568810ba43ULL,  0x2465c3dad80a8fffULL,
    0xcc63f37e1b6a3c43ULL, 0xf1ce60ec7d0343d4ULL, 0x26f9284dbfe0d71bULL,
    0x910cc3453d038620ULL, 0x64378c70287f4c31ULL, 0x5d504ed599cff402ULL,
    0xd9c5589638ae6b23ULL, 0x3dc1ed85bf1bea47ULL, 0x54acdda1f1a214f7ULL,
    0xd234d2076519a026ULL, 0x63cc1080c7ed339cULL, 0x277ee091d8b5f7c9ULL,
    0xc0710e9ac582e536ULL, 0xe1644affb1371f47ULL, 0x6c0afdd74543a81aULL,
    0x4d9bd6c104aa430bULL, 0x29f6494a273c3a62ULL, 0x3468b03d1ae7dec5ULL,
    0x5fbcc88ad6c28b49ULL, 0xad3b957bd21c0576ULL, 0x4e8d95dc0a810be5ULL,
    0x6c64bc46bebbf827ULL, 0x13daf5f6466167deULL, 0x1e62f0a3c370d681ULL,
    0x1f8aa634efeeea5fULL, 0xea06bb99c07cfaccULL, 0xadf6bf8ab83f1e01ULL,
    0x865035c7dcafd0d6ULL, 0xc6a87e35c43bce24ULL, 0x74c4aec64a3016bULL,
    0x5e5d7d5c23121787ULL, 0x567f0b0327c850b1ULL, 0xb0e93674b8c4b464ULL,
    0x92807f5429e78b59ULL, 0xc5b7c713626ff46ULL,  0xf7e44e4a8b0a872ULL,
    0x2d3d3df875f676d3ULL, 0x6427ea97e7c1ab37ULL, 0xa2971ae58139da33ULL,
    0x285e4a07886b205cULL, 0xeece674a0c3c3451ULL, 0xbd372782d96172eaULL,
    0xb082f706c380b46fULL, 0xbb59bf4f9dff391ULL,  0xf8bcefd3bbd44fccULL,
    0x34f110974065583eULL, 0xbe7b8fa05a3c3846ULL, 0x3d35e9bd85206407ULL,
    0x455c501c8e9cd474ULL, 0xdd49d799c314d8abULL, 0xe489b22c28e3d169ULL,
    0x2ab0dffd42938fb7ULL, 0xfb9ddf6e44e6dc38ULL, 0xf212c56d3a0f1e08ULL,
    0xc473372bab45404dULL, 0x61290e9fd9276fdfULL, 0xe7cd3bf022ef8711ULL,
    0xa5f0cb15ac68752fULL, 0xc3974974d1615e5cULL, 0x93085c19d89f27bfULL,
    0x549a4103981c417cULL, 0xa57973c85a16df23ULL, 0xee436e10b75c9a6fULL,
    0xac2c26853027338aULL, 0x82e3f69929b4a714ULL, 0xf1c503fba08822aaULL,
    0xaadd328470f0818dULL, 0xd943ecb8150e94d1ULL, 0x69ed5d7e5a524d2eULL,
    0xf380aa161349a479ULL, 0x135689f24014ef11ULL, 0x8b989932e240ab98ULL,
    0xc8b81039af9c58ebULL, 0x7d223e2194ead544ULL, 0x46bd5d0439898ed9ULL,
    0xfbec2acae9a47350ULL, 0x7590c77dd195620aULL, 0x989d2f9f2ae764fdULL,
    0x8f4955b6e515bedcULL, 0xb5acb484ce92bbddULL, 0x9b2850a69925a3f6ULL,
    0x1210080929193daaULL, 0x5b1dddd2928106a4ULL, 0xc453b6dadb393965ULL,
    0x6a1655edf589657ULL,  0x800d67ef011d51d0ULL, 0xe3617c789a023872ULL,
    0xe1ebfce8356edb29ULL, 0x50ba1d8ebb179379ULL, 0x363a0543927aa87ULL,
    0xbbc2279659f68f24ULL, 0x4031548ad8108637ULL, 0x48fd59702902e687ULL,
    0x7d50f317e67eba54ULL, 0xc9e7c1f918b3875ULL,  0xd0f8b433d0348f83ULL,
    0xe1f41ef89d2197b7ULL, 0x6e18276c61a16677ULL, 0x771ade2e6aebfd82ULL,
    0x5124e0ba4a6a0cddULL, 0x21c71e4a39c7b18eULL, 0x7934411aa796d41bULL,
    0x85e9e708e6f251a2ULL, 0xd11b0bd2a74c7011ULL, 0x7d38bc0ad9282554ULL,
    0x647f4b0aeadc2a5aULL, 0x8694ae01ee10c99dULL, 0xe37a4d9f74837569ULL,
    0x141b039151c12f47ULL, 0xc0a3ce7a0427096fULL, 0x3ea3e911dcdaef5fULL,
    0x2b3587240fbcbba8ULL, 0x26be58e438effba3ULL, 0xc3683148c58be8eaULL,
    0x88658f74fc33642cULL, 0xd737fadcd0cdd51fULL, 0xb198b3d88d2fe25cULL,
    0x4b3e107d801c2586ULL, 0x6990ffbb4daa94ecULL, 0x3d1a83336fea215aULL,
    0x2469f1afad42d3abULL, 0x9969f91677c61efbULL, 0x9280c98a8cca3966ULL,
    0x8eed1cddec928bcdULL, 0xeace1431d9a0b943ULL, 0x2ca880a5f103db1cULL,
    0x2c171122f8dbcbf8ULL, 0xbbc96884bddcb686ULL, 0x683fc52b7e272c4aULL,
    0xc643125fe7d7ae58ULL, 0x7e018dd3bdce334cULL, 0xe733d5ec4e3fbf5fULL,
    0xfbaac9ae8cc1cf40ULL, 0x77b495a13693ef5cULL, 0xcda3cee5be50be73ULL,
    0xec209bcab1a82f28ULL, 0x7ce037b519dfa3efULL, 0x2ea333f59367e1b4ULL,
    0xf28d34e99ef49d07ULL, 0xaaa8ebf0e2aa9f15ULL, 0x33b394ce4403a5d1ULL,
    0xdb20f846b44a1713ULL, 0x7329eb273131fc2aULL, 0x5cc4cc4fd572c0c9ULL,
    0x43373b842e7f00c5ULL, 0xde964940d90a2493ULL, 0x7289ccbc825709f8ULL,
    0x626b887dfa0d7cc0ULL, 0x97fcfffc23422d21ULL, 0x51de3bdeadf83925ULL,
    0xde4f369da282c203ULL, 0xab201e0f47cd98c0ULL, 0x1e761f111f8dc6e4ULL,
    0x26ad0084622ce843ULL, 0x765dc79542104a10ULL, 0x9878f19ce8192b69ULL,
    0xdf686240d2df2b8bULL, 0xf68a9e82b8456ebeULL, 0x43741d758ad609ebULL,
    0xd164844eaa75e47eULL, 0x3eb2b2b67f212055ULL, 0xfc0817ea297dc297ULL,
    0x8b8fba4469bbd098ULL, 0xb98b401141d70713ULL, 0xff4e2a6e1f5600ULL,
    0x2c9d3508be86c4eULL,  0x8ce6afb4e9f31402ULL, 0xe7de9c1b21da6173ULL,
    0x2fdf39c8308ceecfULL, 0xa3cc894311d3223ULL,  0x80d9656c9c30d947ULL,
    0x90e37c54ed6edfd5ULL, 0x93204f3d44f02139ULL, 0xce930bc6c70cf97eULL,
    0xc6ba4117c0d2f679ULL, 0x1d2e1c3ef37386feULL, 0x635ed461f6bcd8f7ULL,
    0xb5598e2e21994c14ULL, 0xa7e80238979f0fccULL, 0x55566183426f65e1ULL,
    0xa89401d4c3682a1ULL,  0x7ce1bc2f0304b6f1ULL, 0xb5f45b7d5d3f21dcULL,
    0x47f25a5430fe72fcULL, 0xa315c28b54ccbcedULL, 0xf7d5b3bd9420aa47ULL,
    0xd7eec223f2ea582fULL, 0xeb37fc40f8df7752ULL, 0x4764e1c2bbc81dd0ULL,
    0x528f573c006f837dULL, 0xfa7e92faa3aedadaULL, 0xec4689830879fab8ULL,
    0x70aaf50fe1763b90ULL, 0x6746c48ed6f96220ULL, 0x7209aaa607164142ULL,
    0xd0f4e9161051e0fcULL, 0xcf43bcd0578c73e2ULL, 0x4e1123dd128b9aabULL,
    0x354f717ce49a8d32ULL, 0x41ddc7d7972e685cULL, 0x422724399d682d6cULL,
    0x74c779897050b92aULL, 0x14c90959225c05eULL,  0xbd1312dbd5921fd4ULL,
    0xbb38a4ae3ae2e343ULL, 0x82d5911f163faf66ULL, 0x434b65fab19c7713ULL,
    0x657beca9def11672ULL, 0x3945de7895aace42ULL, 0x86f6622fee848832ULL,
    0x11aa30921d40bc71ULL, 0x1d6661693bcf6979ULL, 0xbe3020a0e85f88c9ULL,
    0x856980df2a1ca431ULL, 0xc5c81d00d315f9d7ULL, 0x11b6ea06b096c7f8ULL,
    0xacc627cf19dd537fULL, 0x641a5d3c8dc5c169ULL, 0xc30d377c163e68ccULL,
    0x1e13291b72776d46ULL, 0x66481f093131f3f2ULL, 0x546c2f9e09152cefULL,
    0xc42d76252a2cd997ULL, 0xd98cb524eeb2aba4ULL, 0x27e62f9e8ed4420dULL,
    0xb7e57a5331499f9eULL, 0x7fa5873c90b97de2ULL, 0x64267bffb60f927aULL,
    0xd4af4666b7ec91cULL,  0x2e93673ca29d9915ULL, 0x932af23c28d10bfULL,
    0xf4753e48613108ecULL, 0x9c7c369bd0cfaca6ULL, 0x1e4a13dd5810b0e4ULL,
    0xd931a136e80bf386ULL, 0xaeca76fc85118e5dULL, 0x5cd1142ca4ecb184ULL,
    0xb2411a6e2000545ULL,  0x9c7b0713d8d7affdULL, 0x2b6d2ab3aa3b4f6bULL,
    0xe0cf69757f517887ULL, 0x4a2cce4987343280ULL, 0x1a76a33813444dabULL,
    0xb4945766974cf949ULL, 0xb9356b4e1b2fbafcULL, 0x795c2812d0f7cf03ULL,
    0xcd2d57bc0cc3a7acULL, 0x62ffa4be6b4cb280ULL, 0xaa79e70e449925edULL,
    0x709914917cb762ceULL, 0xe9076934da217766ULL, 0x47001bfb05badbf7ULL,
    0xf946b09975ac3d86ULL, 0xa7eeed90aae67af7ULL, 0x942631dc5e54dd33ULL,
    0xf67a85dbb9ac0888ULL, 0x2fbebf2420ae3c06ULL, 0x43766bc391e48becULL,
    0xeeac058583100ee3ULL, 0x13a4a991d72c6278ULL, 0xa89f0b20e0ec95eULL,
    0x3ecf041f12ce8dacULL, 0x34a1af27a8ae53faULL, 0xd881afcb61911e28ULL,
    0xab2d6441f6f9bcc9ULL, 0x2d2c879f450326a7ULL, 0x821e11ccb0c8e55fULL,
    0xd1333baaedafb480ULL, 0xcd8bf9952759bd40ULL, 0x2efd5a0ca37cfd1eULL,
    0x285737f03768da05ULL, 0x2342c5c52fce4e26ULL, 0x244b9abfe8926dbdULL,
    0x35a9726ce4b9fb8dULL, 0xd5c7b056763e8981ULL, 0x408c7be16ef3f007ULL,
    0x548207de27ed6820ULL, 0xf5aac6aec3209c91ULL, 0x137639297ed2eb34ULL,
    0x3540155b8c1b71afULL, 0x9053496e1b421487ULL, 0xc660eb6e33cc704cULL,
    0x57c0ed56cf46acccULL, 0xe9dbad39ccc2f07aULL, 0x1feb72409c3d3456ULL,
    0x76431e4143738f3aULL, 0x1479c096b0f30df0ULL, 0x5d4504a2dcb9185fULL,
    0x93e75483adc87ac8ULL, 0x366a6f4e0ebc09b7ULL, 0x9c4baa5f339afcf5ULL,
    0x4e6fbed39cfc5a12ULL, 0x9d87dbf52a7ef789ULL, 0x2545d36c54bc64d2ULL,
    0x65511035f661e003ULL, 0x3c3786d15df0f80fULL, 0xb79b5b6a322dae7dULL,
    0x9e3991d4c8859b2dULL, 0xa4eaa7560131df1bULL, 0x157009f9f0d62ae6ULL,
    0xc169e0e432ef6e3ULL,  0xfd406af203b9c05bULL, 0xde50c64897809e63ULL,
    0x7044a63c4dcd2d99ULL, 0x9846ed601c9f2000ULL, 0xb89d3c2846daa2acULL,
    0x9c78b9b9ae5fb5ecULL, 0x1a0e0c18b72bdf07ULL, 0xa4b1eb4e1df48416ULL,
    0x84ca788888a86a89ULL, 0xb68ccd64a30941c5ULL, 0xf2907ba4abd4e6c7ULL,
    0xba8ce169b37d2d5dULL, 0x6b5de541aaddd985ULL, 0x2344a712bf96d115ULL,
    0x873aa459dd24bd79ULL, 0xab1ac9b07665be7ULL,  0xde661bd7572487eULL,
    0xe8439e2d519d87e0ULL, 0xabefc7bd25121a4aULL, 0xb250a8a1753ffe5cULL,
    0x735d2dd66ba3e8f2ULL, 0x228e7dbea4e2080dULL, 0x87837e64a535baa4ULL,
    0xc598fa6454470353ULL, 0x7268935eeea54a0aULL, 0x5260e9979be925c4ULL,
    0x8e03a205d6fcc08eULL, 0xcffc9af6252ed405ULL, 0xc4538786b1d6e7dbULL,
    0x1cede3cc7c419e78ULL, 0x4a01b208983943b3ULL, 0x1bd757770d00f633ULL,
    0x3a77cd2ab03e93b0ULL, 0xe180bd6ab693c147ULL, 0x61d9912367286729ULL,
    0x88d8b35731b4d108ULL, 0x43dfc1a54e4705a3ULL, 0x6dd69b38646bd2b3ULL,
    0x1444a485aa66b4d9ULL, 0xa808820948f20052ULL, 0xd9ee9d5adf593a89ULL,
    0xce64c5102899b9deULL, 0xe2c3f9f4970dabedULL, 0x9173cfe315806dbULL,
    0xebc73c8d78674db2ULL, 0x4de82a0f580a7011ULL, 0x38a67e75a4e92827ULL,
    0xf3f05ed31855c3dfULL, 0xf43a71e5be03bdULL,   0x6744d6477a877974ULL,
    0xcf80d356cf12dbffULL, 0xbe238ebaa0c76bc6ULL, 0xd6c60342793407c5ULL,
    0x731e4960d0857d3fULL, 0xeedeb6198f33349ULL,  0x493c6fc6953fc319ULL,
    0x31e0981660d3e622ULL, 0x55369e351b6811bbULL, 0x69eba939c62d4171ULL,
    0xe885b317f472f59ULL,  0x88e5bcf97494acf3ULL, 0x5f2306dfc7fc7e33ULL,
    0x140ba336fbf955c6ULL, 0x9a219e0e5f8c5f8ULL,  0xc4a506cf1c1e87eaULL,
    0xce1310f1c1501b6bULL, 0xea057b698f33f56bULL, 0x86fa9de2f6541d7cULL,
    0x120f5439ab796a96ULL, 0x8fd38c8908b7cee6ULL, 0x1b604dc285ef5232ULL,
    0x95eaa9ca5b03135eULL, 0x5538344677d77b19ULL, 0x443d83ba045f3f6ULL,
    0x61e7faeb516fbc5aULL, 0x428a8f95e2ca4cdbULL, 0xe411fe028a60af37ULL,
    0x39bb03c0c263a8afULL, 0x9ca798209d7c5e45ULL, 0xfd0539e3d7f5b852ULL,
    0x8770917818a701baULL, 0x153c2170d48ae0deULL, 0xa42a92786358c15aULL,
    0xa8f712b636e70c7cULL, 0xece7536ac5182f04ULL, 0xa297e17b0417474aULL,
    0x617e9b55a9d58174ULL, 0x7ccc556aeecdb08dULL, 0x55442ffacb105614ULL,
    0xeba3ae367e4b4d89ULL, 0x7e29f385f859387aULL, 0xef5257e3bcb5e39aULL,
    0x6a589039ccee680eULL, 0xb239dc7e63c815adULL, 0x253998ad8fa4cf34ULL,
    0x1d2edc62b47bdca1ULL, 0xfa4c6a10d8661e7aULL, 0xc9d0f165406445e4ULL,
    0x209f692297e44539ULL, 0x6978d372a7320cecULL, 0xd9905224725ef0d2ULL,
    0x840d78f5fcb8ec1ULL,  0x8d72fe7127210463ULL, 0x2e3b67ef4344ba2bULL,
    0x420add51fde9bd24ULL, 0xc1a166e4fb9e6dc6ULL, 0xd72a98b2b87d5222ULL,
    0x89020c4f7f394e8dULL, 0x8612a5094fe7edeeULL, 0x903968e2f9b6e36dULL,
    0xaf8603fe3154a65bULL, 0x15af941c2432c7d2ULL, 0x90f2d52582667c8dULL,
    0x3b4347906c83530aULL, 0x88363881f621b041ULL, 0xf8d367d63aac6edaULL,
    0x4ff9079121f08efbULL, 0xbdecd4625faf15c4ULL, 0x429bf0f2cbfd4065ULL,
    0x2b369df67d75abe1ULL, 0x85f078442e611b8bULL, 0x8aacd3751775ad28ULL,
    0xef96dfae2a413f78ULL, 0xdae661c2670ec0eeULL, 0x21cb083fdac9b08bULL,
    0x55e18e9a799e4941ULL, 0x339c5abce910a730ULL, 0x8bd0d95beb579bULL,
    0xe7268435c635f7c7ULL, 0x20cc3594b5d9e79dULL, 0x691ccb460b957efaULL,
    0xe157b85e986f8ed5ULL, 0x97f54b326e94543bULL, 0x8af53eca5863243fULL,
    0x6b659fa7bbb7cc0bULL, 0xf740105d5c2625f6ULL, 0x412fe1a8b0babe95ULL,
    0xbddf2b2782d4a81dULL, 0x884dbc5e1aad8b00ULL, 0xd7c7df85d5f59407ULL,
    0x8edb6aabf3722bb7ULL, 0x4af172619b1d941bULL, 0x19a717ccae53e58eULL,
    0xf65e0c4ead290858ULL, 0x52c4ad746821bd28ULL, 0x54a638841f977ea8ULL,
    0x209ef24924ec434eULL, 0x2f8796e7c9c60036ULL, 0x1758bbe7714c30f6ULL,
    0xc87524c2fcc63655ULL, 0xa5d448e8d0d9b704ULL, 0x8637f691396bc2fbULL,
    0x63ef2f5b215d4f59ULL, 0x5967efe30af42cd7ULL, 0x52ee289ac6489e45ULL,
    0xc82fe6a5e84527e0ULL, 0xbda9f0c8680578b1ULL, 0x52f13fb168304d3ULL,
    0xb6ee4d88ab2953dcULL, 0x8c26707e30ed3911ULL, 0x239463e881a6530aULL,
    0xd2a23c14e6156ac7ULL, 0x41c25e02f1d268e3ULL, 0xe94ef67e6301d708ULL,
    0x65d7a62fe67bcf07ULL, 0x7d15ba760f939c57ULL, 0xeabb5772461fb9d8ULL,
    0xabb1a241868ceabdULL, 0x32a6c46bd1a63321ULL, 0xa2318bfb868b2f5aULL,
    0x5a3db89c8cc1787cULL, 0xa7a2c0d0b3722a2aULL, 0xd0cee4528089aa68ULL,
    0xd0d677c910a49d80ULL, 0x79b2d667b39d05dfULL, 0x1e0a58330c21da52ULL,
};

}  // namespace keys

std::uint64_t calculate_hash(const Position &pos);

}  // namespace zobrist

#endif
