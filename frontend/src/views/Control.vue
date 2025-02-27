<template>
    <VContainer class="container" justify="center" align="center">
        <v-row style="max-width: 1200px;" justify="center" class="d-flex flex-wrap ga-12">
            <v-col align="center" class="bg-surface">
                <v-card title="Password" color="surface" subtitle="Enter your four digit passcode" flat >
                    <v-otp-input focus-all :length="4" v-model="passcode"></v-otp-input>
                </v-card>
            </v-col>
        </v-row>
        <v-row>
            <v-col>
                <v-card class="bg-background" flat>
                    <v-btn class="bg-primary" text="Submit" variant="tonal" @click="storePasscode();"></v-btn>
                </v-card>
            </v-col>
        </v-row>
    
    
    </VContainer>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";

import { useAppStore } from "@/store/appStore";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";


 
 
// VARIABLES
const router      = useRouter();
const route       = useRoute();  
const AppStore    = useAppStore();
const Mqtt        = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);
const passcode = ref(["0000"]);


// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout( ()=>{
        // Subscribe to each topic
        Mqtt.subscribe("620157584");
        Mqtt.subscribe("620157584_pub");
    },3000);
});


onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
     Mqtt.unsubcribeAll();
});

function read_passcode() {
    // This reads the passcode 
    console.log(passcode.value);
    AppStore.getSetPwd(passcode.value);
}

</script>


<style scoped>
/** CSS STYLE HERE */


</style>
  