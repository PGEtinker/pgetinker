let fieldId = 0;

/**
 * @param {string} label 
 * @param {string} description
 * @param {(Event) => void} callback 
 * @returns 
 */
export function button(label: string, description: string, callback: (event: Event) => void)
{
    fieldId++;
    
    let group = document.createElement("div");
    group.classList.toggle("form-group", true);

    group.innerHTML = `
        <div class="form-label">${label}</div>
        <div class="form-description">${description}</div>
    `;

    let button = document.createElement("button");
    button.setAttribute("name", `button-${fieldId}`);

    button.innerHTML = label;
    
    button.addEventListener("click", (event) =>
    {
        callback(event);
    });

    group.append(button);

    return group;
}

/**
 * @param {string} label 
 * @param {string} description 
 * @param {(event) => void} valueCallback
 * @param {options[]} options
 * @param {string} initialValue
 * @returns 
 */
export function select(label: string, description: string, valueCallback: (event: Event) => void, options: any, initialValue: string)
{
    fieldId++;
    let select = document.createElement("div")
    
    select.classList.toggle("form-group", true);
    select.classList.toggle("select", true);

    select.innerHTML = `
        <div class="form-label">${label}</div>
        <div class="form-description">${description}</div>
        <select name="select-${fieldId}"></select>
    `;

    for(let i = 0; i < options.length; i++)
    {
        let option = document.createElement("option");
        option.value = options[i].value;
        option.innerHTML = options[i].label;
        
        if(options[i].value == initialValue)
        {
            option.selected = true;
        }

        select.querySelector("select")!.append(option);
    }
    
    select.addEventListener("change", (event) =>
    {
        valueCallback(event);
    });
   
    return select;
}

/**
 * @param {string} label 
 * @param {string} description 
 * @param {(event) => void} callback 
 * @param {boolean} initialValue
 * @returns 
 */
export function toggle(label: string, description: string, callback: (event: Event) => void, initialValue: boolean)
{
    fieldId++;
    let toggle = document.createElement("div")
    
    toggle.classList.toggle("form-group", true);
    toggle.classList.toggle("toggle", true);

    toggle.innerHTML = `
        <div class="form-label">${label}</div>
        <label>
            <input type="checkbox" name="toggle-${fieldId}" ${initialValue ? "checked" : ""}>
            ${description}
        </label>
    `;
    
    toggle.addEventListener("change", (event) =>
    {
        event.preventDefault();
        callback(event);
    });

    return toggle;
}

export function resetFieldId()
{
    fieldId = 0;
}