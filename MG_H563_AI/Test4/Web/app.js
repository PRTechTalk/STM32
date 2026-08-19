const state={leds:[false,false,false]};
const connection=document.querySelector('#connection');

function render(data){
  state.leds=data.leds;
  data.leds.forEach((on,i)=>{
    document.querySelector(`#lamp-${i+1}`).classList.toggle('on',on);
    const button=document.querySelector(`#led-${i+1}`);
    button.setAttribute('aria-pressed',String(on));
    button.textContent=on?'Turn off':'Turn on';
  });
  const input=document.querySelector('#user-button');
  input.classList.toggle('pressed',data.button);
  input.querySelector('strong').textContent=data.button?'Pressed':'Released';
  connection.textContent='Connected';connection.className='badge online';
}

async function refresh(){
  try{const response=await fetch('/api/state',{cache:'no-store'});if(!response.ok)throw new Error(response.status);render(await response.json());}
  catch(error){connection.textContent='Offline';connection.className='badge offline';}
}

for(let i=0;i<3;i++)document.querySelector(`#led-${i+1}`).addEventListener('click',async()=>{
  try{const response=await fetch(`/api/led/${i+1}`,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({on:!state.leds[i]})});if(!response.ok)throw new Error(response.status);render(await response.json());}
  catch(error){connection.textContent='Command failed';connection.className='badge offline';}
});

refresh();setInterval(refresh,500);

